#pragma once

#include <stdint.h>
#include <ztypes.h>

void dbgln(const char* fmt, ...);

// Checks that the code is ok.
// if not exits the process.
void check(uint64_t);

void crash(const char*, z_err_t);
