#include "debug/debug.h"

#include "common/port.h"

#define COM1 0x3f8

void dbgln(const char* str) {
  while (*str != 0) {
    outb(COM1, *str);
    str++;
  }
  outb(COM1, '\n');
}
