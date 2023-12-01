#pragma once

#include <glacier/status/status.h>
#include <glacier/string/str_format.h>
#include <glacier/string/string_view.h>
#include <stdint.h>
#include <ztypes.h>

// TODO: Take StringView here instead.
void dbgln(glcr::StringView string);

template <typename... Args>
void dbgln(const glcr::StringView& fmt, Args... args) {
  dbgln(StrFormat(fmt, args...));
}

// Checks that the code is ok.
// if not exits the process.
void check(uint64_t);
void check(const glcr::Status&);

void crash(const char*, z_err_t);
