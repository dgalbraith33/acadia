#pragma once

#include <glacier/string/str_format.h>
#include <glacier/string/string_builder.h>
#include <stdarg.h>

#include "include/ztypes.h"

void dbgln(const glcr::StringView& str);

template <typename... Args>
void dbgln(const char* str, Args&&... args) {
  char buffer[256];
  glcr::FixedStringBuilder builder(buffer, 256);
  glcr::StrFormatIntoBuffer(builder, str, args...);
  dbgln(builder);
}

template <typename... Args>
void dbgln_large(const char* str, Args&&... args) {
  dbgln(glcr::StrFormat(str, args...));
}

template <typename... Args>
void panic(const char* str, Args&&... args) {
  dbgln(str, args...);
  dbgln("PANIC");
  asm volatile("cli; hlt;");
}

#define PANIC_ON_ERR(expr, str) \
  {                             \
    if (expr != glcr::OK) {     \
      panic(str);               \
    }                           \
  }
#define UNREACHABLE                                                  \
  panic("Unreachable {}, {}", glcr::StringView(__FILE__), __LINE__); \
  __builtin_unreachable();
