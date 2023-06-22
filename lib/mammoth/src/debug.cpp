#include "include/mammoth/debug.h"

#include <glacier/status/error.h>
#include <stdarg.h>
#include <stdio.h>
#include <zcall.h>

void dbgln_internal(const char* str) {  // Safe to ignore the result since right
                                        // now this doesn't throw.
  uint64_t _ = ZDebug(str);
}

void dbgln(const char* fmt, ...) {
  char str[1024];
  va_list arg;
  va_start(arg, fmt);
  int ret = vsprintf(str, fmt, arg);
  va_end(arg);

  if (ret == -1 || ret > 1024) {
    crash("Bad vsprintf", 1);
  }

  dbgln_internal(str);
}

void check(uint64_t code) {
  switch (code) {
    case glcr::OK:
      return;
    case glcr::UNIMPLEMENTED:
      dbgln("crash: UNIMPLEMENTED");
      break;
    case glcr::CAP_NOT_FOUND:
      dbgln("crash: missing capability");
      break;
    case glcr::CAP_WRONG_TYPE:
      dbgln("crash: capability of the wrong type");
      break;
    case glcr::CAP_PERMISSION_DENIED:
      dbgln("crash: capability permissions error");
      break;
    default:
      dbgln("Unhandled code");
      break;
  }
  ZProcessExit(code);
}

void crash(const char* str, uint64_t code) {
  dbgln(str);
  ZProcessExit(code);
}
