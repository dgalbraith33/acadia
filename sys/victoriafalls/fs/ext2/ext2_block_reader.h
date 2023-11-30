#pragma once

#include <denali/denali.yunq.client.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/util/memory_region.h>
#include <yellowstone/yellowstone.yunq.h>

#include "fs/ext2/ext2.h"

/* Simple Wrapper class around the denali client to translate blocks to sectors.
 *
 * By necessity contains the Ext Superblock (to make the translation
 * calculation).
 * */
class Ext2BlockReader {
 public:
  static glcr::ErrorOr<glcr::SharedPtr<Ext2BlockReader>> Init(
      const yellowstone::DenaliInfo& denali_info);

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

  glcr::ErrorOr<mmth::OwnedMemoryRegion> ReadBlock(uint64_t block_number);
  glcr::ErrorOr<mmth::OwnedMemoryRegion> ReadBlocks(uint64_t block_number,
                                                    uint64_t num_blocks);

  glcr::ErrorOr<mmth::OwnedMemoryRegion> ReadBlocks(
      const glcr::Vector<uint64_t>& block_list);

 private:
  DenaliClient denali_;
  uint64_t device_id_;
  uint64_t lba_offset_;
  mmth::OwnedMemoryRegion super_block_region_;

  Ext2BlockReader(DenaliClient&& denali, uint64_t device_id,
                  uint64_t lba_offset, mmth::OwnedMemoryRegion&& super_block);

  uint64_t SectorsPerBlock();
};
