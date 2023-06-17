#pragma once

#include <stdarg.h>

#include "include/ztypes.h"

void dbg(const char* fmt, ...);
void dbgln(const char* str, ...);
void panic(const char* str, ...);

#define RET_ERR(expr)        \
  {                          \
    z_err_t _tmp_err = expr; \
    if (_tmp_err != Z_OK) {  \
      return _tmp_err;       \
    }                        \
  }

#define UNREACHABLE                                \
  panic("Unreachable %s, %s", __FILE__, __LINE__); \
  __builtin_unreachable();
