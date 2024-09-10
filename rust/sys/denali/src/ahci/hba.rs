use bitfield_struct::bitfield;

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
    pub num_ports: u8,

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
    pub hba_reset: bool,
    pub interrupt_enable: bool,

    #[bits(access = RO)]
    pub msi_revert_to_single_message: bool,

    #[bits(28)]
    __: u32,

    pub ahci_enable: bool,
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
#[repr(C)]
pub struct AhciHba {
    pub capabilities: AhciCapabilities,
    pub global_host_control: AhciGlobalControl,
    pub interrupt_status: u32,
    pub port_implemented: u32,
    pub version: u32,
    pub ccc_ctl: u32, // 0x14, Command completion coalescing control
    pub ccc_pts: u32, // 0x18, Command completion coalescing ports
    pub em_loc: u32,  // 0x1C, Enclosure management location
    pub em_ctl: u32,  // 0x20, Enclosure management control
    pub capabilities_ext: AhciCapabilitiesExtended,
    pub bohc: AhciBiosHandoffControl,
}
