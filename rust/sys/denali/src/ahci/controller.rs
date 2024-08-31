use mammoth::mem::MemoryRegion;

use bitfield_struct::bitfield;

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
    __: u32,
    pub subsystem_id: u32,
    pub expansion_rom: u16,
    pub cap_ptr: u8,
    ___: [u8; 7],
    pub interrupt_line: u8,
    pub interrupt_pin: u8,
    pub min_grant: u8,
    pub max_latency: u8,
}

const fn increment(val: u8) -> u8 {
    val + 1
}

#[derive(Debug, PartialEq, Eq)]
#[repr(u8)]
enum InterfaceSpeedSupport {
    Reserved = 0b0000,
    // 1.5 Gbps
    Gen1 = 0b0001,
    // 3 Gbps
    Gen2 = 0b0010,
    // 6 Gbps
    Gen3 = 0b0011,
    Unknown = 0b1111,
}

impl InterfaceSpeedSupport {
    const fn from_bits(value: u8) -> Self {
        match value {
            0b0000 => Self::Reserved,
            0b0001 => Self::Gen1,
            0b0010 => Self::Gen2,
            0b0011 => Self::Gen3,
            _ => Self::Unknown,
        }
    }
}

#[bitfield(u32)]
pub struct AhciCapabilities {
    #[bits(5, access = RO, from = increment)]
    num_ports: u8,

    #[bits(access = RO)]
    supports_external_sata: bool,

    #[bits(access = RO)]
    enclosure_management_supported: bool,

    #[bits(access = RO)]
    command_completed_coalescing_supported: bool,

    #[bits(5, access = RO, from = increment)]
    num_commands: u8,

    #[bits(access = RO)]
    partial_state_capable: bool,

    #[bits(access = RO)]
    slumber_state_capable: bool,

    #[bits(access = RO)]
    pio_multiple_drq_block: bool,

    #[bits(access = RO)]
    fis_based_switching_supported: bool,

    #[bits(access = RO)]
    supports_port_multiplier: bool,

    #[bits(access = RO)]
    supports_ahci_mode_only: bool,

    __: bool,

    #[bits(4, access = RO)]
    interface_speed_support: InterfaceSpeedSupport,

    #[bits(access = RO)]
    supports_command_list_override: bool,

    #[bits(access = RO)]
    supports_activity_led: bool,

    #[bits(access = RO)]
    supports_aggressive_link_power_management: bool,

    #[bits(access = RO)]
    supports_staggered_spin_up: bool,

    #[bits(access = RO)]
    supports_mechanical_presence_switch: bool,

    #[bits(access = RO)]
    supports_snotification_register: bool,

    #[bits(access = RO)]
    supports_native_command_queueing: bool,

    #[bits(access = RO)]
    supports_64_bit_addressing: bool,
}

#[bitfield(u32)]
pub struct AhciGlobalControl {
    hba_reset: bool,
    interrupt_enable: bool,

    #[bits(access = RO)]
    msi_revert_to_single_message: bool,

    #[bits(28)]
    __: u32,

    ahci_enable: bool,
}

#[bitfield(u32)]
pub struct AhciCapabilitiesExtended {
    #[bits(access = RO)]
    bios_os_handoff: bool,

    #[bits(access = RO)]
    nvmhci_present: bool,

    #[bits(access = RO)]
    automatic_partial_to_slumber_transitions: bool,

    #[bits(access = RO)]
    supports_device_sleep: bool,

    #[bits(access = RO)]
    supports_aggressive_device_sleep_management: bool,

    #[bits(27)]
    __: u32,
}

#[bitfield(u32)]
pub struct AhciBiosHandoffControl {
    bios_owned_semaphore: bool,
    os_owned_semaphore: bool,
    smi_on_os_ownership_change_enable: bool,
    os_ownership_change: bool,
    bios_busy: bool,

    #[bits(27)]
    __: u32,
}

#[derive(Debug)]
#[repr(C, packed)]
pub struct AhciHba {
    pub capabilities: AhciCapabilities,
    global_host_control: AhciGlobalControl,
    interrupt_status: u32,
    port_implemented: u32,
    version: u32,
    ccc_ctl: u32, // 0x14, Command completion coalescing control
    ccc_pts: u32, // 0x18, Command completion coalescing ports
    em_loc: u32,  // 0x1C, Enclosure management location
    em_ctl: u32,  // 0x20, Enclosure management control
    capabilities_ext: AhciCapabilitiesExtended,
    bohc: AhciBiosHandoffControl,
}

pub struct AhciController {
    pci_memory: MemoryRegion,
    hba_memory: MemoryRegion,
}

impl AhciController {
    pub fn new(pci_memory: MemoryRegion) -> Self {
        let pci_device_header = unsafe {
            pci_memory
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<PciDeviceHeader>()
                .as_mut()
                .unwrap()
        };
        let hba_memory =
            MemoryRegion::direct_physical(pci_device_header.abar as u64, 0x1100).unwrap();
        Self {
            pci_memory,
            hba_memory,
        }
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

    pub fn ahci_hba(&self) -> &mut AhciHba {
        unsafe {
            self.hba_memory
                .mut_slice::<u8>()
                .as_mut_ptr()
                .cast::<AhciHba>()
                .as_mut()
                .unwrap()
        }
    }
}
