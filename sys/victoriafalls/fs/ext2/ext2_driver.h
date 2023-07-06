#pragma once

#include <denali/denali_client.h>
#include <glacier/memory/move.h>

#include "fs/ext2/ext2.h"

class Ext2Driver {
 public:
  Ext2Driver(ScopedDenaliClient&& denali) : denali_(glcr::Move(denali)) {}

  glcr::ErrorCode ProbePartition();

  glcr::ErrorOr<MappedMemoryRegion> ReadBlocks(uint64_t block_num,
                                               uint64_t num_blocks) {
    return denali_.ReadSectors(SectorsPerBlock() * block_num,
                               SectorsPerBlock() * num_blocks);
  }

  uint64_t SectorsPerBlock() { return 1 << (superblock_->log_block_size + 1); }
  uint64_t BlockToLba(uint64_t block) { return block * SectorsPerBlock(); }
  uint64_t BytesToBlockCount(uint64_t bytes) {
    return (bytes - 1) / (1024 << superblock_->log_block_size) + 1;
  }

 private:
  ScopedDenaliClient denali_;
  Superblock* superblock_;
};
