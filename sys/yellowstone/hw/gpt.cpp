#include "hw/gpt.h"

#include <glacier/memory/move.h>
#include <glacier/status/error.h>
#include <mammoth/debug.h>
#include <mammoth/memory_region.h>
#include <zcall.h>
#include <zglobal.h>

#define GPT_DEBUG 0

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
  ReadRequest req;
  req.set_device_id(0);
  req.set_lba(0);
  req.set_size(2);
  ReadResponse resp;
  RET_ERR(denali_->Read(req, resp));
  MappedMemoryRegion lba_1_and_2 =
      MappedMemoryRegion::FromCapability(resp.memory());
  uint16_t* mbr_sig = reinterpret_cast<uint16_t*>(lba_1_and_2.vaddr() + 0x1FE);
  if (*mbr_sig != 0xAA55) {
    dbgln("Invalid MBR Sig: {x}", *mbr_sig);
    return glcr::FAILED_PRECONDITION;
  }
  MbrPartition* first_partition =
      reinterpret_cast<MbrPartition*>(lba_1_and_2.vaddr() + 0x1BE);
  if (first_partition->boot_indicator != 0) {
    dbgln("Boot indicator set: {}", first_partition->boot_indicator);
    return glcr::FAILED_PRECONDITION;
  }
  if (first_partition->os_type != 0xEE) {
    dbgln("Incorrect OS type: {x}", first_partition->os_type);
    return glcr::FAILED_PRECONDITION;
  }
#if GPT_DEBUG
  dbgln("LBAs: ({x}, {x})", first_partition->starting_lba,
        first_partition->ending_lba);
#endif

  // FIXME: Don't hardcode sector size.
  ParititionHeader* header =
      reinterpret_cast<ParititionHeader*>(lba_1_and_2.vaddr() + 512);

  uint64_t num_partitions = header->num_partitions;
  uint64_t entry_size = header->parition_entry_size;
  uint64_t num_blocks = (num_partitions * entry_size) / 512;

#if GPT_DEBUG
  dbgln("signature {}", header->signature);
  dbgln("lba_partition_entries {x}", header->lba_partition_entries);
  dbgln("num_partitions: {x}", num_partitions);
  dbgln("partition_entry_size: {x}", entry_size);
  dbgln("Num blocks: {x}", num_blocks);
#endif

  req.set_device_id(0);
  req.set_lba(header->lba_partition_entries);
  req.set_size(num_blocks);
  RET_ERR(denali_->Read(req, resp));
  MappedMemoryRegion part_table =
      MappedMemoryRegion::FromCapability(resp.memory());
  for (uint64_t i = 0; i < num_partitions; i++) {
    PartitionEntry* entry = reinterpret_cast<PartitionEntry*>(
        part_table.vaddr() + (i * entry_size));
    if (entry->type_guid_low != 0 || entry->type_guid_high != 0) {
#if GPT_DEBUG
      dbgln("Entry {}", i);
      dbgln("T Guid: {x}-{x}", entry->type_guid_high, entry->type_guid_low);
      dbgln("P Guid: {x}-{x}", entry->part_guid_high, entry->part_guid_low);
      dbgln("LBA: {x}, {x}", entry->lba_start, entry->lba_end);
      dbgln("Attrs: {x}", entry->attributes);
#endif
      // For now we hardcode these values to the type that is
      // created in our setup script.
      // FIXME: Set up our own root partition type guid at some
      // point.
      if (entry->type_guid_low == kLfsDataLow &&
          entry->type_guid_high == kLfsDataHigh) {
        primary_partition_lba_ = entry->lba_start;
      }
    }
  }

  return glcr::OK;
}
