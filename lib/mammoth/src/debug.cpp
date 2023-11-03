#include "include/mammoth/debug.h"

#include <glacier/status/error.h>
#include <zcall.h>

void dbgln(const glcr::StringBuilder& builder) {
  glcr::String str = builder.ToString();
  (void)ZDebug(str.cstr());
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
  (void)ZProcessExit(code);
}

void crash(const char* str, uint64_t code) {
  dbgln(str);
  (void)ZProcessExit(code);
}
