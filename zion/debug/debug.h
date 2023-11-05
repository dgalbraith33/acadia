#pragma once

#include <glacier/string/str_format.h>
#include <stdarg.h>

#include "include/ztypes.h"

// Debug line without formatting for
// before allocations are available.
void early_dbgln(const char* str);

void dbgln(const glcr::StringView& str);

template <typename... Args>
void dbgln(const char* str, Args... args) {
  dbgln(glcr::StrFormat(str, args...));
}

template <typename... Args>
void panic(const char* str, Args... args) {
  dbgln(glcr::StrFormat(str, args...));
  dbgln("PANIC");
}

#define UNREACHABLE                                \
  panic("Unreachable {}, {}", __FILE__, __LINE__); \
  __builtin_unreachable();
