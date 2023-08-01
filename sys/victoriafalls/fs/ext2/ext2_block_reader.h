#pragma once

#include <denali/denali_client.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/memory_region.h>

#include "fs/ext2/ext2.h"

/* Simple Wrapper class around the denali client to translate blocks to sectors.
 *
 * By necessity contains the Ext Superblock (to make the translation
 * calculation).
 * */
class Ext2BlockReader {
 public:
  static glcr::ErrorOr<glcr::SharedPtr<Ext2BlockReader>> Init(
      ScopedDenaliClient&& denali);

  // TODO: Consider creating a new class wrapper with these computations.
  Superblock* GetSuperblock();
  uint64_t BlockSize();
  uint64_t NumberOfBlockGroups();
  uint64_t BgdtBlockNum();
  uint64_t BgdtBlockSize();
  uint64_t InodeSize();
  // FIXME: This probably needs to take into account the block group number
  // because the last table will likely be smaller.
  uint64_t InodeTableBlockSize();

  glcr::ErrorOr<MappedMemoryRegion> ReadBlock(uint64_t block_number);
  glcr::ErrorOr<MappedMemoryRegion> ReadBlocks(uint64_t block_number,
                                               uint64_t num_blocks);

 private:
  ScopedDenaliClient denali_;
  MappedMemoryRegion super_block_region_;

  Ext2BlockReader(ScopedDenaliClient&& denali, MappedMemoryRegion super_block);

  uint64_t SectorsPerBlock();
};
