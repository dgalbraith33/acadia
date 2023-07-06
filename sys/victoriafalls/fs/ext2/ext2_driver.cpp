#include "fs/ext2/ext2_driver.h"

#include <mammoth/debug.h>
glcr::ErrorOr<Ext2Driver> Ext2Driver::Init(ScopedDenaliClient&& denali) {
  ASSIGN_OR_RETURN(Ext2BlockReader reader,
                   Ext2BlockReader::Init(glcr::Move(denali)));
  return Ext2Driver(glcr::Move(reader));
}

glcr::ErrorCode Ext2Driver::ProbePartition() {
  Superblock* superblock = ext2_reader_.GetSuperblock();
  if (superblock->magic != 0xEF53) {
    dbgln("Invalid EXT2 magic code: %x");
    return glcr::INVALID_ARGUMENT;
  }
  dbgln("Block size: 0x%x", 1024 << superblock->log_block_size);

  dbgln("Blocks: 0x%x (0x%x per group)", superblock->blocks_count,
        superblock->blocks_per_group);
  dbgln("Inodes: 0x%x (0x%x per group)", superblock->inodes_count,
        superblock->inodes_per_group);

  dbgln("Mounts: 0x%x out of 0x%x", superblock->mnt_count,
        superblock->max_mnt_count);
  dbgln("State: %x", superblock->state);

  dbgln("Created by: %x", superblock->creator_os);

  uint64_t num_block_groups =
      superblock->blocks_count / superblock->blocks_per_group;
  dbgln("\nReading %u blocks groups", num_block_groups);
  uint64_t bgdt_bytes = sizeof(BlockGroupDescriptor) * num_block_groups;
  uint64_t bgdt_block = (superblock->log_block_size == 0) ? 2 : 1;
  ASSIGN_OR_RETURN(MappedMemoryRegion bgdt,
                   ext2_reader_.ReadBlocks(bgdt_block, bgdt_bytes))

  BlockGroupDescriptor* bgds =
      reinterpret_cast<BlockGroupDescriptor*>(bgdt.vaddr());

  for (uint64_t i = 0; i < num_block_groups; i++) {
    dbgln("BGD %x", i);
    dbgln("Block Bitmap: %x", bgds[i].block_bitmap);
    dbgln("Inode Bitmap: %x", bgds[i].inode_bitmap);
    dbgln("Inode Table: %x", bgds[i].inode_table);
    dbgln("Free blocks: %x", bgds[i].free_blocks_count);
    dbgln("Free inodes: %x", bgds[i].free_inodes_count);
  }

  return glcr::OK;
}
