#pragma once

#include <denali/denali.yunq.client.h>
#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error.h>
#include <stdint.h>
#include <ztypes.h>

class GptReader {
 public:
  GptReader(glcr::UniquePtr<DenaliClient> denali);

  glcr::ErrorCode ParsePartitionTables();

  uint64_t GetPrimaryPartitionLba() { return primary_partition_lba_; }

 private:
  glcr::UniquePtr<DenaliClient> denali_;
  uint64_t primary_partition_lba_;
};
