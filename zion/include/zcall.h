#pragma once

#include <stdint.h>

#define Z_THREAD_EXIT 0x01
#define Z_DEBUG_PRINT 0x100

uint64_t ZDebug(const char* message);
