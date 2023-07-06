#include "fs/ext2/ext2_driver.h"

#include <mammoth/debug.h>

glcr::ErrorCode Ext2Driver::ProbePartition() {
  // Read 1024 bytes from 1024 offset.
  // FIXME: Don't assume 512 byte sectors somehow.
  ASSIGN_OR_RETURN(MappedMemoryRegion superblock, denali_.ReadSectors(2, 2));
  superblock_ = reinterpret_cast<Superblock*>(superblock.vaddr());

  if (superblock_->magic != 0xEF53) {
    dbgln("Invalid EXT2 magic code: %x");
    return glcr::INVALID_ARGUMENT;
  }
  dbgln("Block size: 0x%x", 1024 << superblock_->log_block_size);

  dbgln("Blocks: 0x%x (0x%x per group)", superblock_->blocks_count,
        superblock_->blocks_per_group);
  dbgln("Inodes: 0x%x (0x%x per group)", superblock_->inodes_count,
        superblock_->inodes_per_group);

  dbgln("Mounts: 0x%x out of 0x%x", superblock_->mnt_count,
        superblock_->max_mnt_count);
  dbgln("State: %x", superblock_->state);

  dbgln("Created by: %x", superblock_->creator_os);

  uint64_t num_block_groups =
      superblock_->blocks_count / superblock_->blocks_per_group;
  dbgln("\nReading %u blocks groups", num_block_groups);
  uint64_t bgdt_bytes = sizeof(BlockGroupDescriptor) * num_block_groups;
  uint64_t bgdt_block = (superblock_->log_block_size == 0) ? 2 : 1;
  ASSIGN_OR_RETURN(MappedMemoryRegion bgdt,
                   ReadBlocks(bgdt_block, BytesToBlockCount(bgdt_bytes)));

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

  uint64_t root_inode = 2;
  return glcr::OK;
}
