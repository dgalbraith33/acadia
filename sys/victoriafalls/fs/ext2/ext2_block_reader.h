#pragma once

#include <denali/denali_client.h>
#include <glacier/status/error_or.h>
#include <mammoth/memory_region.h>

#include "fs/ext2/ext2.h"

class Ext2BlockReader {
 public:
  static glcr::ErrorOr<Ext2BlockReader> Init(ScopedDenaliClient&& denali);

  Superblock* GetSuperblock();
  uint64_t SectorsPerBlock();

  glcr::ErrorOr<MappedMemoryRegion> ReadBlock(uint64_t block_number);
  glcr::ErrorOr<MappedMemoryRegion> ReadBlocks(uint64_t block_number,
                                               uint64_t num_blocks);

 private:
  ScopedDenaliClient denali_;
  MappedMemoryRegion super_block_region_;

  Ext2BlockReader(ScopedDenaliClient&& denali, MappedMemoryRegion super_block);
};
