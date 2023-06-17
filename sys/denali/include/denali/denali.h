#pragma once

#include <stdint.h>

#define DENALI_INVALID 0
#define DENALI_READ 100

struct DenaliRead {
  uint64_t request_type = DENALI_READ;
  uint64_t device_id;

  uint64_t lba;
  uint64_t size;
};

// Will also include a memory capability.
struct DenaliReadResponse {
  uint64_t device_id;
  uint64_t lba;
  uint64_t size;
};
