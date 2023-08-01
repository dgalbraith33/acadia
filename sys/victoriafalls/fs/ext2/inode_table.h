#pragma once

#include <glacier/container/vector.h>
#include <stdint.h>

#include "fs/ext2/ext2_block_reader.h"

class InodeTable {
 public:
  InodeTable(const glcr::SharedPtr<Ext2BlockReader>& driver,
             BlockGroupDescriptor* bgdt);

  glcr::ErrorOr<Inode*> GetInode(uint32_t inode_num);

 private:
  glcr::SharedPtr<Ext2BlockReader> ext2_reader_;
  BlockGroupDescriptor* bgdt_;

  glcr::Vector<MappedMemoryRegion> inode_tables_;

  glcr::ErrorOr<Inode*> GetRootOfInodeTable(uint64_t block_group_num);
};
