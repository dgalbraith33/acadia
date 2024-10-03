#[derive(Debug)]
#[repr(C, packed)]
pub struct CommandHeader {
    pub command: u16,
    pub prd_table_length: u16,
    pub prd_byte_count: u32,
    pub command_table_base_addr: u64,
    __: u64,
    ___: u64,
}

pub type CommandList = [CommandHeader; 32];

#[allow(dead_code)]
#[derive(Clone, Copy, Debug, PartialEq)]
#[repr(u8)]
pub enum FisType {
    RegisterHostToDevice = 0x27, // Register FIS - host to device
    RegisterDeviceToHost = 0x34, // Register FIS - device to host
    DmaActivate = 0x39,          // DMA activate FIS - device to host
    DmaSetup = 0x41,             // DMA setup FIS - bidirectional
    Data = 0x46,                 // Data FIS - bidirectional
    BistActivate = 0x58,         // BIST activate FIS - bidirectional
    PioSetup = 0x5F,             // PIO setup FIS - device to host
    SetDeviceBits = 0xA1,        // Set device bits FIS - device to host
    Unknown = 0x0,
}

#[allow(dead_code)]
#[derive(Copy, Clone, Debug)]
#[repr(C, packed)]
pub struct DmaFis {
    // DWORD 0
    fis_type: u8,   // FIS_TYPE_DMA_SETUP
    pmport_dia: u8, // Port multiplier

    __: u16, // Reserved

    // DWORD 1&2
    dma_buffer_id: u64, // DMA Buffer Identifier. Used to Identify DMA buffer
    // in host memory. SATA Spec says host specific and not
    // in Spec. Trying AHCI spec might work.

    // DWORD 3
    ___: u32,

    // DWORD 4
    dma_buffer_offest: u32, // Byte offset into buffer. First 2 bits must be 0

    // DWORD 5
    transfer_count: u32, // Number of bytes to transfer. Bit 0 must be 0

    // DWORD 6
    ____: u32, // Reserved
}

const _: () = assert!(size_of::<DmaFis>() == 28);

#[allow(dead_code)]
#[derive(Copy, Clone, Debug)]
#[repr(C, packed)]
pub struct PioSetupFis {
    // DWORD 0
    fis_type: FisType, // FIS_TYPE_PIO_SETUP

    pmport_di: u8, // Port multiplier

    status: u8, // Status register
    error: u8,  // Error register

    // DWORD 1
    lba0: u8,   // LBA low register, 7:0
    lba1: u8,   // LBA mid register, 15:8
    lba2: u8,   // LBA high register, 23:16
    device: u8, // Device register

    // DWORD 2
    lba3: u8, // LBA register, 31:24
    lba4: u8, // LBA register, 39:32
    lba5: u8, // LBA register, 47:40
    __: u8,   // Reserved

    // DWORD 3
    countl: u8,   // Count register, 7:0
    counth: u8,   // Count register, 15:8
    ___: u8,      // Reserved
    e_status: u8, // New value of status register

    // DWORD 4
    tc: u16,   // Transfer count
    ____: u16, // Reserved
}

const _: () = assert!(size_of::<PioSetupFis>() == 20);

#[derive(Clone, Copy, Debug)]
#[repr(u8)]
pub enum SataCommand {
    IdentifyDevice = 0xEC,
    DmaReadExt = 0x25,
}

#[allow(dead_code)] // Read by memory.
#[derive(Clone, Copy, Debug)]
#[repr(C)]
pub struct HostToDeviceRegisterFis {
    fis_type: FisType, // FIS_TYPE_REG_H2D
    pmp_and_c: u8,
    command: SataCommand, // Command register
    featurel: u8,         // Feature register, 7:0

    // DWORD 1
    lba0: u8,   // LBA low register, 7:0
    lba1: u8,   // LBA mid register, 15:8
    lba2: u8,   // LBA high register, 23:16
    device: u8, // Device register

    // DWORD 2
    lba3: u8,     // LBA register, 31:24
    lba4: u8,     // LBA register, 39:32
    lba5: u8,     // LBA register, 47:40
    featureh: u8, // Feature register, 15:8

    // DWORD 3
    count: u16,
    icc: u8,     // Isochronous command completion
    control: u8, // Control register

    // DWORD 4
    reserved: u32, // Reserved
}

const _: () = assert!(size_of::<HostToDeviceRegisterFis>() == 20);

impl HostToDeviceRegisterFis {
    pub fn new_command(command: SataCommand, lba: u64, sectors: u16) -> Self {
        Self {
            fis_type: FisType::RegisterHostToDevice,
            pmp_and_c: 0x80, // Set command bit
            command,
            featurel: 0,

            lba0: (lba & 0xFF) as u8,
            lba1: ((lba >> 8) & 0xFF) as u8,
            lba2: ((lba >> 16) & 0xFF) as u8,
            device: (1 << 6), // ATA LBA Mode

            lba3: ((lba >> 24) & 0xFF) as u8,
            lba4: ((lba >> 32) & 0xFF) as u8,
            lba5: ((lba >> 40) & 0xFF) as u8,
            featureh: 0,

            count: sectors,
            icc: 0,
            control: 0,

            reserved: 0,
        }
    }
}

#[derive(Copy, Clone, Debug)]
#[repr(C, packed)]
pub struct DeviceToHostRegisterFis {
    // DWORD 0
    pub fis_type: FisType, // FIS_TYPE_REG_D2H

    pub pmport_and_i: u8,

    pub status: u8, // Status register
    pub error: u8,  // Error register

    // DWORD 1
    pub lba0: u8,   // LBA low register, 7:0
    pub lba1: u8,   // LBA mid register, 15:8
    pub lba2: u8,   // LBA high register, 23:16
    pub device: u8, // Device register

    // DWORD 2
    pub lba3: u8, // LBA register, 31:24
    pub lba4: u8, // LBA register, 39:32
    pub lba5: u8, // LBA register, 47:40
    __: u8,

    // DWORD 3
    pub count: u16,
    ___: u16,

    ____: u32,
}

const _: () = assert!(size_of::<DeviceToHostRegisterFis>() == 20);

#[derive(Copy, Clone, Debug)]
#[repr(C, packed)]
pub struct SetDeviceBitsFis {
    fis_type: FisType,
    pmport_and_i: u8,
    status: u8,
    error: u8,
    reserved: u32,
}

const _: () = assert!(size_of::<SetDeviceBitsFis>() == 8);

#[derive(Debug)]
#[repr(C, packed)]
pub struct ReceivedFis {
    pub dma_fis: DmaFis,
    __: u32,

    pub pio_set_fis: PioSetupFis,
    ___: [u8; 12],

    pub device_to_host_register_fis: DeviceToHostRegisterFis,
    ____: u32,

    pub set_device_bits_fis: SetDeviceBitsFis,

    pub unknown_fis: [u8; 64],
}

const _: () = assert!(size_of::<ReceivedFis>() == 0xA0);

#[derive(Copy, Clone)]
#[repr(C)]
pub union CommandFis {
    pub host_to_device: HostToDeviceRegisterFis,

    // Used to ensure the repr is
    pub __: [u8; 64],
}

const _: () = assert!(size_of::<CommandFis>() == 0x40);

impl core::fmt::Debug for CommandFis {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        f.write_fmt(format_args!("{:?}", unsafe { self.host_to_device }))
    }
}

#[derive(Copy, Clone, Debug)]
#[repr(C, packed)]
pub struct PhysicalRegionDescriptor {
    pub region_address: u64,
    __: u32,
    // bit 0 must be one.
    // 21:0 is byte count
    // 31 is Interrupt on Completion
    pub byte_count: u32,
}

const _: () = assert!(size_of::<PhysicalRegionDescriptor>() == 0x10);

#[derive(Debug)]
#[repr(C, packed)]
pub struct CommandTable {
    pub command_fis: CommandFis,
    pub atapi_command: [u8; 0x10],
    __: [u8; 0x30],
    pub prdt: [PhysicalRegionDescriptor; 8],
}

const _: () = assert!(size_of::<CommandTable>() == 0x100);
