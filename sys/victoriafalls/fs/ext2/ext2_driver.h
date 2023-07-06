#pragma once

#include <denali/denali_client.h>
#include <glacier/memory/move.h>

#include "fs/ext2/ext2.h"
#include "fs/ext2/ext2_block_reader.h"
#include "fs/ext2/inode_table.h"

class Ext2Driver {
 public:
  static glcr::ErrorOr<Ext2Driver> Init(ScopedDenaliClient&& denali);

  glcr::ErrorCode ProbePartition();

  glcr::ErrorCode ProbeDirectory(Inode* inode);

 private:
  Ext2BlockReader ext2_reader_;
  glcr::UniquePtr<InodeTable> inode_table_;

  Ext2Driver(Ext2BlockReader&& reader) : ext2_reader_(glcr::Move(reader)) {}
};
