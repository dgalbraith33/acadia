#include "string.h"

void* memcpy(void* dest, const void* src, size_t count) {
  uint8_t* d = (uint8_t*)dest;
  const uint8_t* s = (uint8_t*)src;
  for (size_t i = 0; i < count; i++) {
    d[i] = s[i];
  }
  return dest;
}
