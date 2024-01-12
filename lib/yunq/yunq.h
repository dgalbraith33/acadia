#pragma once

#include <stdint.h>

namespace yunq {

struct MessageHeader {
  uint32_t ident;
  uint32_t core_length;
  uint32_t length;
  uint32_t crc32;
  uint64_t options;
} __attribute__((packed));

const uint64_t kHeaderSize = 24;  // 4x uint32, 1x uint64

struct ExtensionPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace yunq
