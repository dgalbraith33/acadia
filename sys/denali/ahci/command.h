#pragma once

#include <stdint.h>

struct CommandInfo {
  uint8_t command;
  uint64_t lba;
  uint16_t sectors;
  uint64_t paddr;
};
