#pragma once

#include <denali/denali_client.h>
#include <glacier/memory/move.h>

#include "fs/ext2/ext2.h"
#include "fs/ext2/ext2_block_reader.h"

class Ext2Driver {
 public:
  static glcr::ErrorOr<Ext2Driver> Init(ScopedDenaliClient&& denali);

  glcr::ErrorCode ProbePartition();

 private:
  Ext2BlockReader ext2_reader_;

  Ext2Driver(Ext2BlockReader&& reader) : ext2_reader_(glcr::Move(reader)) {}
};
