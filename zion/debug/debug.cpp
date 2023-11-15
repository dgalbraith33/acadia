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

void dbg(const glcr::StringView& str) {
  for (uint64_t i = 0; i < str.size(); i++) {
    dbgputchar(str[i]);
  }
}

void AddProcPrefix() {
  if (gScheduler != nullptr) {
    auto t = gScheduler->CurrentThread();

    char buffer[16];
    glcr::FixedStringBuilder builder(buffer, 16);
    glcr::StrFormatIntoBuffer(builder, "[{}.{}] ", t->pid(), t->tid());
    dbg(builder);
  }
}

}  // namespace

void dbgln(const glcr::StringView& str) {
  AddProcPrefix();
  dbg(str);
  dbg("\n");
}
