#include "include/zcall.h"

#include <stdint.h>

#include "usr/zcall_internal.h"

z_err_t SysCall1(uint64_t number, const void* first) {
  z_err_t return_code;
  asm("syscall" : "=a"(return_code) : "D"(number), "S"(first) : "rcx", "r11");
  return return_code;
}
