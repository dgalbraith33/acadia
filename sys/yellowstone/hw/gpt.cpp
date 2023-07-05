#include "hw/gpt.h"

#include <glacier/status/error.h>
#include <mammoth/debug.h>
#include <zcall.h>
#include <zglobal.h>

const uint64_t kSectorSize = 512;

const uint64_t kGptPartitionSignature = 0x54524150'20494645;

const uint64_t kLfsDataLow = 0x477284830fc63daf;
const uint64_t kLfsDataHigh = 0xe47d47d8693d798e;

struct MbrPartition {
  uint8_t boot_indicator;
  uint8_t starting_chs[3];
  uint8_t os_type;
  uint8_t ending_chs[3];
  uint32_t starting_lba;
  uint32_t ending_lba;
} __attribute__((packed));

struct ParititionHeader {
  uint64_t signature;
  uint32_t revision;
  uint32_t header_size;
  uint32_t crc_32;
  uint32_t reserved;
  uint64_t lba_self;
  uint64_t lba_mirror;
  uint64_t lba_min;
  uint64_t lba_max;
  uint64_t guid_low;
  uint64_t guid_high;
  uint64_t lba_partition_entries;
  uint32_t num_partitions;
  uint32_t parition_entry_size;
  uint32_t partition_entry_crc32;
} __attribute__((packed));

struct PartitionEntry {
  uint64_t type_guid_low;
  uint64_t type_guid_high;
  uint64_t part_guid_low;
  uint64_t part_guid_high;
  uint64_t lba_start;
  uint64_t lba_end;
  uint64_t attributes;
  char partition_name[72];
} __attribute__((packed));

GptReader::GptReader(glcr::UniquePtr<DenaliClient> denali)
    : denali_(glcr::Move(denali)) {}

glcr::ErrorCode GptReader::ParsePartitionTables() {
  ASSIGN_OR_RETURN(MappedMemoryRegion lba_1_and_2,
                   denali_->ReadSectors(0, 0, 2));
  uint16_t* mbr_sig = reinterpret_cast<uint16_t*>(lba_1_and_2.vaddr() + 0x1FE);
  if (*mbr_sig != 0xAA55) {
    dbgln("Invalid MBR Sig: %x", *mbr_sig);
    return glcr::FAILED_PRECONDITION;
  }
  MbrPartition* first_partition =
      reinterpret_cast<MbrPartition*>(lba_1_and_2.vaddr() + 0x1BE);
  if (first_partition->boot_indicator != 0) {
    dbgln("Boot indicator set: %u", first_partition->boot_indicator);
    return glcr::FAILED_PRECONDITION;
  }
  if (first_partition->os_type != 0xEE) {
    dbgln("Incorrect OS type: %x", first_partition->os_type);
    return glcr::FAILED_PRECONDITION;
  }
  dbgln("LBAs: (%x, %x)", first_partition->starting_lba,
        first_partition->ending_lba);

  // FIXME: Don't hardcode sector size.
  ParititionHeader* header =
      reinterpret_cast<ParititionHeader*>(lba_1_and_2.vaddr() + 512);

  dbgln("signature %lx", header->signature);

  uint64_t num_partitions = header->num_partitions;
  uint64_t entry_size = header->parition_entry_size;
  uint64_t num_blocks = (num_partitions * entry_size) / 512;

  dbgln("lba_partition_entries %lx", header->lba_partition_entries);
  dbgln("num_partitions: %x", num_partitions);
  dbgln("partition_entry_size: %x", entry_size);
  dbgln("Num blocks: %x", num_blocks);

  ASSIGN_OR_RETURN(
      MappedMemoryRegion part_table,
      denali_->ReadSectors(0, header->lba_partition_entries, num_blocks));
  dbgln("Entries");
  for (uint64_t i = 0; i < num_partitions; i++) {
    PartitionEntry* entry = reinterpret_cast<PartitionEntry*>(
        part_table.vaddr() + (i * entry_size));
    if (entry->type_guid_low != 0 || entry->type_guid_high != 0) {
      dbgln("Entry %u", i);
      dbgln("T Guid: %lx-%lx", entry->type_guid_high, entry->type_guid_low);
      dbgln("P Guid: %lx-%lx", entry->part_guid_high, entry->part_guid_low);
      dbgln("LBA: %lx, %lx", entry->lba_start, entry->lba_end);
      dbgln("Attrs: %lx", entry->attributes);
      // For now we hardcode these values to the type that is created in our
      // setup script.
      // FIXME: Set up our own root partition type guid at some point.
      if (entry->type_guid_low == kLfsDataLow &&
          entry->type_guid_high == kLfsDataHigh) {
        primary_partition_lba_ = entry->lba_start;
      }
    }
  }

  return glcr::OK;
}
