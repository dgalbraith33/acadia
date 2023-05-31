#pragma once

#include <stdarg.h>

void dbg(const char* fmt, ...);
void dbgln(const char* str, ...);
void panic(const char* str, ...);
