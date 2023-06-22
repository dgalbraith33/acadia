#pragma once

#include <stdarg.h>

#include "include/ztypes.h"

void dbg(const char* fmt, ...);
void dbgln(const char* str, ...);
void panic(const char* str, ...);

#define UNREACHABLE                                \
  panic("Unreachable %s, %s", __FILE__, __LINE__); \
  __builtin_unreachable();
