use core::cell::RefCell;

use alloc::rc::Rc;
use alloc::{collections::BTreeMap, string::String};
use mammoth::{cap::Capability, mem::MemoryRegion, zion::ZError};
use victoriafalls::VFSClient;
use yellowstone_yunq::{
    AhciInfo, DenaliInfo, Endpoint, FramebufferInfo, GetEndpointRequest, RegisterEndpointRequest,
    XhciInfo, YellowstoneServerHandler,
};

use crate::pci::PciReader;

pub struct YellowstoneServerContext {
    registration_semaphore: mammoth::sync::Semaphore,
    pci_reader: PciReader,
    framebuffer_info_region: MemoryRegion,
    service_map: RefCell<BTreeMap<String, Capability>>,
}

impl YellowstoneServerContext {
    fn framebuffer_info(&self) -> yellowstone_yunq::FramebufferInfo {
        let fb_info: &mammoth::zion::ZFramebufferInfo = unsafe {
            self.framebuffer_info_region
                .slice::<u8>()
                .as_ptr()
                .cast::<mammoth::zion::ZFramebufferInfo>()
                .as_ref()
                .unwrap()
        };

        yellowstone_yunq::FramebufferInfo {
            address_phys: fb_info.address_phys,
            width: fb_info.width,
            height: fb_info.height,
            pitch: fb_info.pitch,
            bpp: fb_info.bpp as u64,
            memory_model: fb_info.memory_model as u64,
            red_mask_size: fb_info.red_mask_size as u64,
            red_mask_shift: fb_info.red_mask_shift as u64,
            blue_mask_size: fb_info.blue_mask_size as u64,
            blue_mask_shift: fb_info.blue_mask_shift as u64,
            green_mask_size: fb_info.green_mask_size as u64,
            green_mask_shift: fb_info.green_mask_shift as u64,
        }
    }
}

impl YellowstoneServerContext {
    pub fn new(pci_region: MemoryRegion, fb_region: MemoryRegion) -> Result<Self, ZError> {
        Ok(Self {
            registration_semaphore: mammoth::sync::Semaphore::new()?,
            pci_reader: PciReader::new(pci_region),
            framebuffer_info_region: fb_region,
            service_map: BTreeMap::new().into(),
        })
    }

    pub fn wait(&self, service: &str) -> Result<(), ZError> {
        loop {
            match self.service_map.borrow().get(service) {
                Some(_) => return Ok(()),
                None => {}
            }
            self.registration_semaphore.wait().unwrap();
        }
    }
}

pub struct YellowstoneServerImpl {
    context: Rc<YellowstoneServerContext>,
}

impl YellowstoneServerImpl {
    pub fn new(context: Rc<YellowstoneServerContext>) -> Self {
        Self { context }
    }
}

impl YellowstoneServerHandler for YellowstoneServerImpl {
    fn register_endpoint(&mut self, req: RegisterEndpointRequest) -> Result<(), ZError> {
        if req.endpoint_name == "victoriafalls" {
            victoriafalls::set_client(VFSClient::new(
                Capability::take_copy(req.endpoint_capability).unwrap(),
            ));
        }

        self.context
            .service_map
            .borrow_mut()
            .insert(req.endpoint_name, Capability::take(req.endpoint_capability));

        self.context.registration_semaphore.signal()?;
        Ok(())
    }

    fn get_endpoint(&mut self, req: GetEndpointRequest) -> Result<Endpoint, ZError> {
        match self.context.service_map.borrow().get(&req.endpoint_name) {
            Some(cap) => Ok(Endpoint {
                endpoint: cap.duplicate(Capability::PERMS_ALL)?.release(),
            }),
            None => Err(ZError::NOT_FOUND),
        }
    }

    fn get_ahci_info(&mut self) -> Result<AhciInfo, ZError> {
        Ok(AhciInfo {
            ahci_region: self.context.pci_reader.get_ahci_region()?.release(),
            region_length: 0x1000,
        })
    }

    fn get_xhci_info(&mut self) -> Result<XhciInfo, ZError> {
        Ok(XhciInfo {
            xhci_region: self.context.pci_reader.get_xhci_region()?.release(),
            region_length: 0x1000,
        })
    }

    fn get_framebuffer_info(&mut self) -> Result<FramebufferInfo, ZError> {
        Ok(self.context.framebuffer_info())
    }

    fn get_denali(&mut self) -> Result<DenaliInfo, ZError> {
        match self.context.service_map.borrow().get("denali") {
            Some(ep_cap) => crate::gpt::read_gpt(denali::DenaliClient::new(
                ep_cap.duplicate(Capability::PERMS_ALL).unwrap(),
            ))
            .map(|lba| DenaliInfo {
                denali_endpoint: ep_cap.duplicate(Capability::PERMS_ALL).unwrap().release(),
                device_id: 0,
                lba_offset: lba,
            }),
            None => {
                mammoth::debug!("Denali not yet registered");
                Err(ZError::FAILED_PRECONDITION)
            }
        }
    }
}
