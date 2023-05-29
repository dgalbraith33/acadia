#include "include/zcall.h"

#include <stdint.h>

uint64_t SysCall1(uint64_t number, const void* first) {
  uint64_t return_code;
  asm("syscall" : "=a"(return_code) : "D"(number), "S"(first) : "rcx", "r11");
  return return_code;
}

uint64_t ZDebug(const char* message) {
  return SysCall1(Z_DEBUG_PRINT, message);
}
