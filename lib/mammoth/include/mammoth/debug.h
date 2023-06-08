#pragma once

#include <stdint.h>
#include <zerrors.h>

void dbgln(const char* fmt, ...);

// Checks that the code is ok.
// if not exits the process.
void check(uint64_t);

void crash(const char*, z_err_t);

#define RET_ERR(expr)        \
  {                          \
    z_err_t _tmp_err = expr; \
    if (_tmp_err != Z_OK) {  \
      return _tmp_err;       \
    }                        \
  }
