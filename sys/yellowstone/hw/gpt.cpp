#include "hw/gpt.h"

#include <mammoth/debug.h>
#include <zcall.h>
#include <zglobal.h>

struct MbrPartition {
  uint8_t boot_indicator;
  uint8_t starting_chs[3];
  uint8_t os_type;
  uint8_t ending_chs[3];
  uint32_t starting_lba;
  uint32_t ending_lba;
} __attribute__((packed));

z_err_t CheckMbrIsGpt(uint64_t mem_cap) {
  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, &vaddr));
  uint16_t* mbr_sig = reinterpret_cast<uint16_t*>(vaddr + 0x1FE);
  if (*mbr_sig != 0xAA55) {
    return Z_ERR_INVALID;
  }

  MbrPartition* first_partition =
      reinterpret_cast<MbrPartition*>(vaddr + 0x1BE);
  if (first_partition->boot_indicator != 0) {
    dbgln("Boot indicator set: %u", first_partition->boot_indicator);
    return Z_ERR_INVALID;
  }
  if (first_partition->os_type != 0xEE) {
    dbgln("Incorrect OS type: %x", first_partition->os_type);
  }
  dbgln("LBAs: (%x, %x)", first_partition->starting_lba,
        first_partition->ending_lba);
  return Z_OK;
}

const uint64_t kGptPartitionSignature = 0x54524150'20494645;
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

z_err_t ReadPartitionHeader(uint64_t mem_cap) {
  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, &vaddr));
  ParititionHeader* header = reinterpret_cast<ParititionHeader*>(vaddr);

  dbgln("signature %lx", header->signature);

  dbgln("lba_partition_entries %lx", header->lba_partition_entries);
  dbgln("num_partitions: %x", header->num_partitions);
  dbgln("partition_entry_size: %x", header->parition_entry_size);
  dbgln("Num blocks: %x",
        (header->num_partitions * header->parition_entry_size) / 512);

  dbgln("LBA: (%x, %x)", header->lba_min, header->lba_max);

  return Z_OK;
}

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

z_err_t ReadPartitionEntries(uint64_t mem_cap) {
  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(gSelfVmasCap, 0, mem_cap, &vaddr));
  ParititionHeader* header = reinterpret_cast<ParititionHeader*>(vaddr);

  // FIXME: Dont hard code these.
  uint64_t num_parts = 0x28;
  uint64_t entry_size = 0x80;

  dbgln("Entries");
  for (uint64_t i = 0; i < num_parts; i++) {
    PartitionEntry* entry =
        reinterpret_cast<PartitionEntry*>(vaddr + (i * entry_size));
    if (entry->type_guid_low != 0 || entry->type_guid_high != 0) {
      dbgln("Entry %u", i);
      dbgln("T Guid: %lx-%lx", entry->type_guid_high, entry->type_guid_low);
      dbgln("P Guid: %lx-%lx", entry->part_guid_high, entry->part_guid_low);
      dbgln("LBA: %lx, %lx", entry->lba_start, entry->lba_end);
      dbgln("Attrs: %lx", entry->attributes);
    }
  }

  return Z_OK;
}
