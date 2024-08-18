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
    denali_semaphore: mammoth::sync::Semaphore,
    victoria_falls_semaphore: mammoth::sync::Semaphore,
    pci_reader: PciReader,
}

impl YellowstoneServerContext {
    pub fn new(pci_region: MemoryRegion) -> Result<Self, ZError> {
        Ok(Self {
            denali_semaphore: mammoth::sync::Semaphore::new()?,
            victoria_falls_semaphore: mammoth::sync::Semaphore::new()?,
            pci_reader: PciReader::new(pci_region),
        })
    }

    pub fn wait_denali(&self) -> Result<(), ZError> {
        self.denali_semaphore.wait()
    }

    pub fn wait_victoria_falls(&self) -> Result<(), ZError> {
        self.victoria_falls_semaphore.wait()
    }
}

pub struct YellowstoneServerImpl {
    context: Rc<YellowstoneServerContext>,
    service_map: BTreeMap<String, Capability>,
}

impl YellowstoneServerImpl {
    pub fn new(context: Rc<YellowstoneServerContext>) -> Self {
        Self {
            context,
            service_map: BTreeMap::new(),
        }
    }
}

impl YellowstoneServerHandler for YellowstoneServerImpl {
    fn register_endpoint(&mut self, req: RegisterEndpointRequest) -> Result<(), ZError> {
        let signal_denali = req.endpoint_name == "denali";
        let signal_vfs = req.endpoint_name == "victoriafalls";

        let raw_cap = req.endpoint_capability;

        self.service_map
            .insert(req.endpoint_name, Capability::take(req.endpoint_capability));

        if signal_denali {
            self.context.denali_semaphore.signal()?;
        }
        if signal_vfs {
            self.context.victoria_falls_semaphore.signal()?;
            victoriafalls::set_client(VFSClient::new(Capability::take_copy(raw_cap).unwrap()));
        }
        Ok(())
    }

    fn get_endpoint(&mut self, req: GetEndpointRequest) -> Result<Endpoint, ZError> {
        match self.service_map.get(&req.endpoint_name) {
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
        todo!()
    }

    fn get_framebuffer_info(&mut self) -> Result<FramebufferInfo, ZError> {
        todo!()
    }

    fn get_denali(&mut self) -> Result<DenaliInfo, ZError> {
        match self.service_map.get("denali") {
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
