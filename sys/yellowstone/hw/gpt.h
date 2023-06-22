#pragma once

#include <denali/denali_client.h>
#include <stdint.h>
#include <ztypes.h>

class GptReader {
 public:
  GptReader(const DenaliClient&);

  glcr::ErrorCode ParsePartitionTables();

 private:
  DenaliClient denali_;
};
