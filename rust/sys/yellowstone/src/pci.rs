use mammoth::{cap::Capability, mem::MemoryRegion, zion::ZError};

pub struct PciReader {
    memory_region: MemoryRegion,
}

type DevPredicate = fn(u8, u8, u8) -> bool;

impl PciReader {
    pub fn new(memory_region: MemoryRegion) -> Self {
        Self { memory_region }
    }

    pub fn get_ahci_region(&self) -> Result<Capability, ZError> {
        match self.probe_pci(|class, _, _| class == 0x1) {
            Some(m) => Ok(m),
            None => Err(ZError::NOT_FOUND),
        }
    }

    pub fn get_xhci_region(&self) -> Result<Capability, ZError> {
        match self.probe_pci(|class, subclass, prog_interface| {
            class == 0xC && subclass == 0x3 && prog_interface == 0x30
        }) {
            Some(m) => Ok(m),
            None => Err(ZError::NOT_FOUND),
        }
    }

    fn probe_pci(&self, pred: DevPredicate) -> Option<Capability> {
        let base_header = self.pci_header(0, 0, 0);
        if (base_header.header_type & 0x80) == 0 {
            if let Some(dev) = self.probe_bus(pred, 0) {
                return Some(dev);
            }
        } else {
            for fun in 0..8 {
                let fun_hdr = self.pci_header(0, 0, fun);
                if fun_hdr.vendor_id != 0xFFFF {
                    if let Some(dev) = self.probe_bus(pred, fun) {
                        return Some(dev);
                    }
                }
            }
        }
        None
    }

    fn probe_bus(&self, pred: DevPredicate, bus: u8) -> Option<Capability> {
        for dev in 0..0x20 {
            if let Some(dev) = self.probe_device(pred, bus, dev) {
                return Some(dev);
            }
        }
        None
    }

    fn probe_device(&self, pred: DevPredicate, bus: u8, dev: u8) -> Option<Capability> {
        let device_base_header = self.pci_header(bus, dev, 0);
        if device_base_header.vendor_id == 0xFFFF {
            return None;
        }

        if let Some(dev) = self.probe_function(pred, bus, dev, 0) {
            return Some(dev);
        }

        if (device_base_header.header_type & 0x80) != 0 {
            for fun in 1..8 {
                if let Some(dev) = self.probe_function(pred, bus, dev, fun) {
                    return Some(dev);
                }
            }
        }
        None
    }

    fn probe_function(&self, pred: DevPredicate, bus: u8, dev: u8, fun: u8) -> Option<Capability> {
        let function_header = self.pci_header(bus, dev, fun);

        mammoth::debug!(
            "PCI Function: {:#x} {:#x} {:#x}",
            function_header.class_code,
            function_header.subclass,
            function_header.prog_interface
        );

        if pred(
            function_header.class_code,
            function_header.subclass,
            function_header.prog_interface,
        ) {
            mammoth::debug!("Found!");
            let offset = pci_header_offset(bus, dev, fun);
            Some(
                self.memory_region
                    .duplicate(offset as u64, 0x1000)
                    .expect("Failed to duplicate PCI cap"),
            )
        } else {
            None
        }
    }

    fn pci_header(&self, bus: u8, dev: u8, fun: u8) -> &PciHeader {
        let offset = pci_header_offset(bus, dev, fun);
        let header_slice: &[u8] =
            &self.memory_region.slice()[offset..offset + size_of::<PciHeader>()];
        unsafe { header_slice.as_ptr().cast::<PciHeader>().as_ref().unwrap() }
    }
}

#[repr(C, packed)]
struct PciHeader {
    vendor_id: u16,
    device_id: u16,
    command_reg: u16,
    status_reg: u16,
    revision: u8,
    prog_interface: u8,
    subclass: u8,
    class_code: u8,
    cache_line_size: u8,
    latency_timer: u8,
    header_type: u8,
    bist: u8,
}

fn pci_header_offset(bus: u8, dev: u8, fun: u8) -> usize {
    ((bus as usize) << 20) | ((dev as usize) << 15) | ((fun as usize) << 12)
}
