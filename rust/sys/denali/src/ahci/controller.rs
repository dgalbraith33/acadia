use mammoth::mem::MemoryRegion;

#[repr(C, packed)]
pub struct PciDeviceHeader {
    pub vendor_id: u16,
    pub device_id: u16,
    pub command_reg: u16,
    pub status_reg: u16,
    pub revision: u8,
    pub prog_interface: u8,
    pub subclass: u8,
    pub class_code: u8,
    pub cache_line_size: u8,
    pub latency_timer: u8,
    pub header_type: u8,
    pub bist: u8,
    pub bars: [u32; 5],
    pub abar: u32,
    pub reserved0: u32,
    pub subsystem_id: u32,
    pub expansion_rom: u16,
    pub cap_ptr: u8,
    pub reserved1: [u8; 7],
    pub interrupt_line: u8,
    pub interrupt_pin: u8,
    pub min_grant: u8,
    pub max_latency: u8,
}

pub struct AhciController {
    pci_memory: MemoryRegion,
}

impl AhciController {
    pub fn new(pci_memory: MemoryRegion) -> Self {
        Self { pci_memory }
    }

    pub fn pci_header(&self) -> &mut PciDeviceHeader {
        unsafe {
            self.pci_memory
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<PciDeviceHeader>()
                .as_mut()
                .unwrap()
        }
    }
}
