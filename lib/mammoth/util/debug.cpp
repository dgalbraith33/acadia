#include "util/debug.h"

#include <glacier/status/error.h>
#include <zcall.h>

void dbgln(glcr::StringView string) {
  (void)ZDebug(string.data(), string.size());
}

void check(uint64_t code) {
  if (code == glcr::OK) {
    return;
  }
  dbgln("crash!");
  (void)ZProcessExit(code);
}

void crash(const char* str, uint64_t code) {
  dbgln(str);
  (void)ZProcessExit(code);
}
