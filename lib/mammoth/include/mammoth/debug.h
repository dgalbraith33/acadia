#pragma once

#include <stdint.h>

void dbgln(const char*);

// Checks that the code is ok.
// if not exits the process.
void check(uint64_t);
