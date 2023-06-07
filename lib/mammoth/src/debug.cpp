#include "include/mammoth/debug.h"

#include <zcall.h>
#include <zerrors.h>

void dbgln(const char* str) {
  // Safe to ignore the result since right now this doesn't throw.
  uint64_t _ = ZDebug(str);
}

void check(uint64_t code) {
  switch (code) {
    case Z_OK:
      return;
    case ZE_NOT_FOUND:
      dbgln("crash: NOT_FOUND");
      break;
    case ZE_INVALID:
      dbgln("crash: INVALID");
      break;
    case ZE_DENIED:
      dbgln("crash: DENIED");
      break;
    case ZE_UNIMPLEMENTED:
      dbgln("crash: UNIMPLEMENTED");
    default:
      dbgln("Unhandled code");
      break;
  }
  ZProcessExit(code);
}
