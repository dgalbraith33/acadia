#include "debug/debug.h"

#include "common/port.h"
#include "scheduler/scheduler.h"

#define COM1 0x3f8

namespace {
bool is_transmit_empty() { return (inb(COM1 + 5) & 0x20) != 0; }

void dbgputchar(char c) {
  while (!is_transmit_empty())
    ;

  outb(COM1, c);
}

void dbgcstr(const char* str) {
  for (; *str != '\0'; str++) {
    dbgputchar(*str);
  }
}

void U64ToStr(uint64_t u, char* str) {
  uint64_t len = 0;
  uint64_t u2 = u;
  while (u2 != 0) {
    len++;
    u2 /= 10;
  }

  if (len == 0) {
    len = 1;
  }

  str[len] = '\0';

  for (int64_t i = len - 1; i >= 0; i--) {
    str[i] = (u % 10) + '0';
    u /= 10;
  }
}

void HexToStr(uint64_t u, char* str) {
  uint64_t len = 0;
  uint64_t u2 = u;
  while (u2 != 0) {
    len++;
    u2 /= 16;
  }

  if (len == 0) {
    len = 1;
  }

  len += 2;
  str[0] = '0';
  str[1] = 'x';
  str[len] = '\0';
  const char* hex = "0123456789ABCDEF";
  for (uint64_t i = len - 1; i > 1; i--) {
    str[i] = hex[u & 0xF];
    u >>= 4;
  }
}

void MemToStr(uint64_t u, char* str) {
  str[0] = '0';
  str[1] = 'x';
  const char* hex = "0123456789ABCDEF";
  for (uint64_t i = 0; i < 16; i++) {
    str[17 - i] = hex[(u >> (i * 4)) & 0xF];
  }
  str[18] = '\0';
}

void AddProcPrefix() {
  if (gScheduler != nullptr) {
    auto& t = gScheduler->CurrentThread();
    dbg("[%u.%u] ", t.pid(), t.tid());
  }
}

void dbg_internal(const char* fmt, va_list args) {
  for (; *fmt != '\0'; fmt++) {
    if (*fmt != '%') {
      dbgputchar(*fmt);
      continue;
    }
    fmt++;
    switch (*fmt) {
      case '%':
        dbgputchar('%');
        break;
      case 's': {
        char* str = va_arg(args, char*);
        dbgcstr(str);
        break;
      }
      case 'c': {
        char c = va_arg(args, int);
        dbgputchar(c);
        break;
      }
      case 'u': {
        uint64_t u = va_arg(args, uint64_t);
        char str[21];
        U64ToStr(u, str);
        dbgcstr(str);
        break;
      }
      case 'x': {
        uint64_t u = va_arg(args, uint64_t);
        char str[19];
        HexToStr(u, str);
        dbgcstr(str);
        break;
      }
      case 'm': {
        uint64_t u = va_arg(args, uint64_t);
        char str[19];
        MemToStr(u, str);
        dbgcstr(str);
        break;
      }
      default: {
        panic("Bad format char: %c", *fmt);
      }
    }
  }
}

}  // namespace

void dbg(const char* fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  dbg_internal(fmt, arg);
  va_end(arg);
}

void dbgln(const char* fmt, ...) {
  AddProcPrefix();
  va_list arg;
  va_start(arg, fmt);
  dbg_internal(fmt, arg);
  va_end(arg);
  dbgputchar('\n');
}

void panic(const char* fmt, ...) {
  asm volatile("cli");
  AddProcPrefix();
  va_list arg;
  va_start(arg, fmt);
  dbg_internal(fmt, arg);
  va_end(arg);
  dbgln("\nPANIC");
  while (1)
    ;
}
