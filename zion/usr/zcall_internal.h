#pragma once

#include <stdint.h>

#include "include/ztypes.h"

struct ZMessage {
  uint64_t num_bytes;
  void* data;

  uint64_t num_caps;
  z_cap_t* caps;
};
