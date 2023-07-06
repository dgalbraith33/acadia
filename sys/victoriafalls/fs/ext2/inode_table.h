#pragma once

#include <glacier/container/vector.h>
#include <stdint.h>

#include "fs/ext2/ext2_block_reader.h"

class InodeTable {
 public:
  InodeTable(Ext2BlockReader& driver, BlockGroupDescriptor* bgdt);

  glcr::ErrorOr<Inode*> GetInode(uint64_t inode_num);

 private:
  Ext2BlockReader& ext2_reader_;
  BlockGroupDescriptor* bgdt_;

  glcr::Vector<MappedMemoryRegion> inode_tables_;

  glcr::ErrorOr<Inode*> GetRootOfInodeTable(uint64_t block_group_num);
};
