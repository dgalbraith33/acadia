#pragma once

#include <stdarg.h>

#include "include/ztypes.h"

// Debug line without formatting for
// before allocations are available.
void early_dbgln(const char* str);

void dbg(const char* fmt, ...);
void dbgln(const char* str, ...);
void panic(const char* str, ...);

#define UNREACHABLE                                \
  panic("Unreachable %s, %s", __FILE__, __LINE__); \
  __builtin_unreachable();
