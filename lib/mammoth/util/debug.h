#pragma once

#include <glacier/string/str_format.h>
#include <glacier/string/string_view.h>
#include <stdint.h>
#include <ztypes.h>

// TODO: Take StringView here instead.
void dbgln(const glcr::String& string);

template <typename... Args>
void dbgln(const glcr::StringView& fmt, Args... args) {
  dbgln(StrFormat(fmt, args...));
}

// Checks that the code is ok.
// if not exits the process.
void check(uint64_t);

void crash(const char*, z_err_t);
