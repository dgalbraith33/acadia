#include "include/zcall.h"

#include <stdint.h>

#include "usr/zcall_internal.h"

z_err_t SysCall2(uint64_t number, const void* first, const void* second) {
  z_err_t return_code;
  asm("syscall"
      : "=a"(return_code)
      : "D"(number), "S"(first), "d"(second)
      : "rcx", "r11");
  return return_code;
}

z_err_t SysCall0(uint64_t number) { return SysCall2(number, 0, 0); }

z_err_t SysCall1(uint64_t number, const void* first) {
  return SysCall2(number, first, 0);
}

z_err_t ZCapDuplicate(z_cap_t cap_in, z_cap_t* cap_out) {
  ZCapDuplicateReq req{
      .cap = cap_in,
  };
  ZCapDuplicateResp resp;
  z_err_t ret = SysCall2(Z_CAP_DUPLICATE, &req, &resp);
  *cap_out = resp.cap;
  return ret;
}

z_err_t ZDebug(const char* message) { return SysCall1(Z_DEBUG_PRINT, message); }
