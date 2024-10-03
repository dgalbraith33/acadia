use bitfield_struct::bitfield;

#[bitfield(u32)]
pub struct AhciPortInterruptStatus {
    pub device_to_host_register_fis_interrupt: bool,
    pub pio_setup_fis_interrupt: bool,
    pub dma_setup_fis_interrupt: bool,
    pub set_device_bits_interrupt: bool,
    #[bits(access = RO)]
    pub unknown_fis_interrupt: bool,
    pub descriptor_prossed: bool,
    #[bits(access = RO)]
    pub port_connect_change_status: bool,
    pub device_mechanical_presence_status: bool,

    #[bits(14)]
    __: u32,

    #[bits(access = RO)]
    pub phy_rdy_change_status: bool,
    pub incorrect_port_multiplier_status: bool,
    pub overflow_status: bool,

    __: bool,
    pub interface_non_fatal_error_status: bool,
    pub interface_fatal_error_status: bool,
    pub host_bus_data_error_status: bool,
    pub host_bus_fatal_error_status: bool,
    pub task_file_error_status: bool,
    pub cold_port_detect_status: bool,
}

#[bitfield(u32)]
pub struct AhciPortInterruptEnable {
    pub device_to_host_register_fis_enable: bool,
    pub pio_setup_fis_enable: bool,
    pub dma_setup_fis_enable: bool,
    pub set_device_bits_fis_enable: bool,
    pub unknown_fis_enable: bool,
    pub descriptor_processed_enable: bool,
    pub port_change_enable: bool,
    pub device_mechanical_presence_enable: bool,

    #[bits(14)]
    __: u32,

    pub phy_rdy_change_enable: bool,
    pub incorrect_port_multiplier_enable: bool,
    pub overflow_enable: bool,

    __: bool,

    pub interface_non_fatal_error_enable: bool,
    pub interface_fatal_error_enable: bool,
    pub host_bus_data_error_enable: bool,
    pub host_bust_fatal_error_enable: bool,
    pub task_file_error_enable: bool,
    pub cold_presence_detect_enable: bool,
}

#[repr(u8)]
#[derive(Debug)]
enum InterfaceCommunicationControl {
    NoOpOrIdle = 0x0,
    Active = 0x1,
    Partial = 0x2,
    Slumber = 0x6,
    DevSleep = 0x8,
    Unknown = 0xF,
}

impl InterfaceCommunicationControl {
    const fn from_bits(value: u8) -> Self {
        match value {
            0x0 => Self::NoOpOrIdle,
            0x1 => Self::Active,
            0x2 => Self::Partial,
            0x6 => Self::Slumber,
            0x8 => Self::DevSleep,
            _ => Self::Unknown,
        }
    }

    const fn into_bits(self) -> u8 {
        self as _
    }
}

#[bitfield(u32)]
pub struct AhciPortCommandAndStatus {
    pub start: bool,
    pub spin_up_device: bool,
    pub power_on_device: bool,
    pub command_list_overide: bool,
    pub fis_recieve_enable: bool,

    #[bits(3)]
    __: u8,

    #[bits(5, access = RO)]
    pub current_command_slot: u8,

    #[bits(access = RO)]
    pub mechanical_presence_switch_state: bool,
    #[bits(access = RO)]
    pub fis_receive_running: bool,
    #[bits(access = RO)]
    pub command_list_running: bool,
    #[bits(access = RO)]
    pub cold_presence_state: bool,
    pub port_multipler_attached: bool,
    #[bits(access = RO)]
    pub hot_plug_capable_port: bool,
    #[bits(access = RO)]
    pub mechanical_presence_switch_attached_to_port: bool,
    #[bits(access = RO)]
    pub cold_presence_detection: bool,
    #[bits(access = RO)]
    pub external_sata_port: bool,
    #[bits(access = RO)]
    pub fis_base_switch_capable: bool,
    pub automatic_partial_to_slumber_transitions_enable: bool,
    pub device_is_atapi: bool,
    pub drive_led_on_atapi_enable: bool,
    pub aggressive_power_link_management_enable: bool,
    pub aggressive_slumber_partial: bool,

    #[bits(4)]
    pub interface_communication_control: InterfaceCommunicationControl,
}

#[bitfield(u32)]
pub struct AhciPortTaskFileData {
    #[bits(access = RO)]
    err_status: bool,
    #[bits(2, access = RO)]
    command_specific_status_lo: u8,
    #[bits(access = RO)]
    data_transfer_requested: bool,
    #[bits(3, access = RO)]
    command_specific_status_hi: u8,
    #[bits(access = RO)]
    busy_status: bool,

    #[bits(8, access = RO)]
    error: u8,

    #[bits(16)]
    __: u16,
}

#[derive(Copy, Clone, Debug, PartialEq)]
#[repr(u8)]
pub enum AhciDeviceDetection {
    NoDevice = 0x0,
    NoCommunication = 0x1,
    CommunicationEstablished = 0x3,
    OfflineMode = 0x4,
    Unknown = 0xF,
}

impl AhciDeviceDetection {
    const fn from_bits(value: u8) -> Self {
        match value {
            0x0 => Self::NoDevice,
            0x1 => Self::NoCommunication,
            0x3 => Self::CommunicationEstablished,
            0x4 => Self::OfflineMode,
            _ => Self::Unknown,
        }
    }
}

#[derive(Copy, Clone, Debug)]
#[repr(u8)]
pub enum AhciCurrentInterfaceSpeed {
    NoDevice = 0x0,
    Gen1 = 0x1,
    Gen2 = 0x2,
    Gen3 = 0x3,
    Unknown = 0xF,
}

impl AhciCurrentInterfaceSpeed {
    const fn from_bits(value: u8) -> Self {
        match value {
            0x0 => Self::NoDevice,
            0x1 => Self::Gen1,
            0x2 => Self::Gen2,
            0x3 => Self::Gen3,
            _ => Self::Unknown,
        }
    }
}

#[derive(Copy, Clone, Debug, PartialEq)]
#[repr(u8)]
pub enum AhciInterfacePowerManagement {
    NoDevice = 0x0,
    Active = 0x1,
    PartialPower = 0x2,
    Slumber = 0x6,
    DevSleep = 0x8,
    Unknown = 0xF,
}

impl AhciInterfacePowerManagement {
    const fn from_bits(value: u8) -> Self {
        match value {
            0x0 => Self::NoDevice,
            0x1 => Self::Active,
            0x2 => Self::PartialPower,
            0x6 => Self::Slumber,
            0x8 => Self::DevSleep,
            _ => Self::Unknown,
        }
    }
}

#[bitfield(u32)]
#[derive(PartialEq)]
pub struct AhciSataStatus {
    #[bits(4, access = RO)]
    pub device_detection: AhciDeviceDetection,

    #[bits(4, access = RO)]
    pub current_interface_speed: AhciCurrentInterfaceSpeed,

    #[bits(4, access = RO)]
    pub interface_power_management: AhciInterfacePowerManagement,

    #[bits(20)]
    __: u32,
}

#[derive(Debug)]
#[repr(u8)]
enum AhciDeviceDetectionInitialization {
    NoDevice = 0x0,
    PerformInterfaceCommunicationInitializationSequence = 0x1,
    DisableSata = 0x4,
    Unknown = 0xF,
}

impl AhciDeviceDetectionInitialization {
    const fn into_bits(self) -> u8 {
        self as _
    }

    const fn from_bits(value: u8) -> Self {
        match value {
            0x0 => Self::NoDevice,
            0x1 => Self::PerformInterfaceCommunicationInitializationSequence,
            0x4 => Self::DisableSata,
            _ => Self::Unknown,
        }
    }
}

#[derive(Debug)]
#[repr(u8)]
enum AhciSpeedAllowed {
    NoRestrictions = 0x0,
    LimitGen1 = 0x1,
    LimitGen2 = 0x2,
    LimitGen3 = 0x3,
    Unknown = 0xF,
}

impl AhciSpeedAllowed {
    const fn into_bits(self) -> u8 {
        self as _
    }

    const fn from_bits(value: u8) -> Self {
        match value {
            0x0 => Self::NoRestrictions,
            0x1 => Self::LimitGen1,
            0x2 => Self::LimitGen2,
            0x3 => Self::LimitGen3,
            _ => Self::Unknown,
        }
    }
}

#[bitfield(u32)]
pub struct AhciSataControl {
    #[bits(4)]
    device_detection_initialization: AhciDeviceDetectionInitialization,

    #[bits(4)]
    speed_allowed: AhciSpeedAllowed,

    partial_transition_disabled: bool,
    slumber_transition_disabled: bool,
    devsleep_transition_disabled: bool,

    __: bool,

    #[bits(20)]
    __: u32,
}

#[bitfield(u32)]
pub struct AhciSataError {
    pub recovered_data_integrity_error: bool,
    pub recovered_communications_error: bool,

    #[bits(6)]
    __: u8,

    pub transient_data_integrity_error: bool,
    pub persisten_communication_or_data_integrity_error: bool,
    pub protocol_error: bool,
    pub internal_error: bool,

    #[bits(4)]
    __: u8,

    pub phy_ready_change: bool,
    pub phy_internal_error: bool,
    pub comm_wake: bool,
    pub decode_error: bool,
    __: bool,
    pub crc_error: bool,
    pub handshake_error: bool,
    pub link_sequence_error: bool,
    pub transport_state_transition_error: bool,
    pub uknown_fis_type: bool,
    pub exchanged: bool,

    #[bits(5)]
    __: u8,
}

#[bitfield(u32)]
pub struct AhciFisBasedSwitchingControl {
    enable: bool,
    device_error_clear: bool,

    #[bits(access = RO)]
    single_device_error: bool,

    #[bits(5)]
    __: u8,

    #[bits(4)]
    device_to_issue: u8,

    #[bits(4, access = RO)]
    active_device_optimization: u8,

    #[bits(4, access = RO)]
    device_with_error: u8,

    #[bits(12)]
    __: u16,
}

#[bitfield(u32)]
pub struct AhciDeviceSleep {
    aggressive_device_sleep_enable: bool,

    #[bits(access = RO)]
    device_sleep_present: bool,

    device_sleep_exit_timeout: u8,

    #[bits(5)]
    minimum_device_sleep_assertion_time: u8,

    #[bits(10)]
    device_sleep_idle_timeout: u16,

    #[bits(4)]
    dito_multiplier: u8,

    #[bits(3)]
    __: u8,
}

#[derive(Debug)]
#[repr(C, packed)]
pub struct AhciPortHba {
    pub command_list_base: u64,
    pub fis_base: u64,
    pub interrupt_status: AhciPortInterruptStatus,
    pub interrupt_enable: AhciPortInterruptEnable,
    pub command: AhciPortCommandAndStatus,
    __: u32,
    pub task_file_data: AhciPortTaskFileData,
    pub signature: u32,
    pub sata_status: AhciSataStatus,
    pub sata_control: AhciSataControl,
    pub sata_error: AhciSataError,
    pub sata_active: u32,
    pub command_issue: u32,
    pub sata_notification: u32,
    pub fis_based_switching_ctl: AhciFisBasedSwitchingControl,
    pub device_sleep: AhciDeviceSleep,
}

const _: () = assert!(size_of::<AhciPortHba>() == 0x48);
