#pragma once

#include <glacier/container/hash_map.h>
#include <glacier/memory/move.h>
#include <glacier/memory/unique_ptr.h>
#include <yellowstone/yellowstone.yunq.h>

#include "fs/ext2/ext2.h"
#include "fs/ext2/ext2_block_reader.h"
#include "fs/ext2/inode_table.h"

class Ext2Driver {
 public:
  static glcr::ErrorOr<Ext2Driver> Init(
      const yellowstone::DenaliInfo& denali_info);

  glcr::ErrorCode ProbePartition();

  glcr::ErrorOr<Inode*> GetInode(uint32_t inode_number);

  glcr::ErrorOr<glcr::Vector<DirEntry>> ReadDirectory(uint32_t inode_number);

  glcr::ErrorOr<mmth::OwnedMemoryRegion> ReadFile(uint64_t inode_number);

 private:
  glcr::SharedPtr<Ext2BlockReader> ext2_reader_;
  glcr::UniquePtr<InodeTable> inode_table_;
  glcr::HashMap<uint64_t, mmth::OwnedMemoryRegion> inode_cache_;

  Ext2Driver(const glcr::SharedPtr<Ext2BlockReader>& reader,
             glcr::UniquePtr<InodeTable> inode_table)
      : ext2_reader_(reader), inode_table_(glcr::Move(inode_table)) {}

  glcr::ErrorOr<mmth::OwnedMemoryRegion> ReadInode(uint64_t inode_num,
                                                   Inode* inode);
};
