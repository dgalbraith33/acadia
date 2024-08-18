use denali::DenaliClient;
use mammoth::{cap::Capability, zion::ZError};

const MBR_SIG: u16 = 0xAA55;

const GPT_OS_TYPE: u8 = 0xEE;

#[repr(C, packed)]
struct MbrPartition {
    boot_indicator: u8,
    starting_chs: [u8; 3],
    os_type: u8,
    ending_chs: [u8; 3],
    starting_lba: u32,
    ending_lba: u32,
}

#[repr(C, packed)]
struct PartitionHeader {
    signature: u64,
    revision: u32,
    header_size: u32,
    crc_32: u32,
    reserved: u32,
    lba_self: u64,
    lba_mirror: u64,
    lba_min: u64,
    lba_max: u64,
    guid_low: u64,
    guid_high: u64,
    lba_partition_entries: u64,
    num_partitions: u32,
    partition_entry_size: u32,
    partition_entry_crc32: u32,
}

#[repr(C, packed)]
struct PartitionEntry {
    type_guid_low: u64,
    type_guid_high: u64,
    part_guid_low: u64,
    part_guid_high: u64,
    lba_start: u64,
    lba_end: u64,
    attrs: u64,
    partition_name: [u8; 72],
}

pub fn read_gpt(mut denali: DenaliClient) -> Result<u64, ZError> {
    let resp = denali.read(&denali::ReadRequest {
        device_id: 0,
        block: denali::DiskBlock { lba: 0, size: 2 },
    })?;

    let first_lbas = mammoth::mem::MemoryRegion::from_cap(Capability::take(resp.memory))?;

    let maybe_mbr_sig = u16::from_le_bytes(first_lbas.slice()[0x1FE..0x200].try_into().unwrap());

    if maybe_mbr_sig != MBR_SIG {
        mammoth::debug!("MBR Sig {:#x} does not match {:#x}", maybe_mbr_sig, MBR_SIG);
        return Err(ZError::FAILED_PRECONDITION);
    }

    let mbr = unsafe {
        first_lbas.slice::<u8>()[0x1BE..]
            .as_ptr()
            .cast::<MbrPartition>()
            .as_ref()
            .unwrap()
    };

    if mbr.os_type != GPT_OS_TYPE {
        let os_type = mbr.os_type;
        mammoth::debug!(
            "MBR OS Type: {:#x} does not match GPT ({:#x})",
            os_type,
            GPT_OS_TYPE
        );
    }

    let gpt_header = unsafe {
        first_lbas.slice::<u8>()[512..]
            .as_ptr()
            .cast::<PartitionHeader>()
            .as_ref()
            .unwrap()
    };

    let num_partitions = gpt_header.num_partitions;
    let partition_entry_size = gpt_header.partition_entry_size;
    let lba_partition_entries = gpt_header.lba_partition_entries;
    let num_blocks = (num_partitions * partition_entry_size).div_ceil(512) as u64;

    mammoth::debug!(
        "Reading partition table from LBA {}, {} entries of size {} equals {} blocks.",
        lba_partition_entries,
        num_partitions,
        partition_entry_size,
        num_blocks
    );

    let resp = denali.read(&denali::ReadRequest {
        device_id: 0,
        block: denali::DiskBlock {
            lba: lba_partition_entries,
            size: num_blocks,
        },
    })?;

    let partition_table = mammoth::mem::MemoryRegion::from_cap(Capability::take(resp.memory))?;

    for ind in 0..num_partitions {
        let offset = (ind * partition_entry_size) as usize;
        let limit = offset + (partition_entry_size as usize);

        let partition_entry = unsafe {
            partition_table.slice::<u8>()[offset..limit]
                .as_ptr()
                .cast::<PartitionEntry>()
                .as_ref()
                .unwrap()
        };

        if partition_entry.type_guid_low == 0 && partition_entry.type_guid_high == 0 {
            continue;
        }

        const LFS_DATA_LOW: u64 = 0x477284830fc63daf;
        const LFS_DATA_HIGH: u64 = 0xe47d47d8693d798e;
        if partition_entry.type_guid_low == LFS_DATA_LOW
            && partition_entry.type_guid_high == LFS_DATA_HIGH
        {
            return Ok(partition_entry.lba_start);
        }
    }

    Err(ZError::NOT_FOUND)
}
