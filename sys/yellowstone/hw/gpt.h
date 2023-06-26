#pragma once

#include <denali/denali_client.h>
#include <stdint.h>
#include <ztypes.h>

class GptReader {
 public:
  GptReader(glcr::UniquePtr<DenaliClient> denali);

  glcr::ErrorCode ParsePartitionTables();

 private:
  glcr::UniquePtr<DenaliClient> denali_;
};
