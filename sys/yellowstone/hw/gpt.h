#pragma once

#include <denali/denali_client.h>
#include <stdint.h>
#include <ztypes.h>

class GptReader {
 public:
  GptReader(const DenaliClient&);

  z_err_t ParsePartitionTables();

 private:
  DenaliClient denali_;
};
