#pragma once

#include <glacier/string/str_format.h>
#include <stdarg.h>

#include "include/ztypes.h"

// Debug line without formatting for
// before allocations are available.
void early_dbgln(const char* str);

void dbgln(const glcr::StringView& str);

// TODO: Write a version of StrFormat that
// accepts a fix-sized buffer for output
// to use in the kernel. That way we make
// dbgln and panic calls without allocation.
// Optionally, add a dbgln_unbounded method for
// things like the Debug syscall where the formatted
// string may be fairly large.

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
