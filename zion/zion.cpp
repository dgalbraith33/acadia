#include <stdint.h>

#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"

extern "C" void zion() {
  dbgln("Hello World!");
  InitGdt();
  dbgln("New GDT Loaded!");
  InitIdt();
  dbgln("IDT Loaded!");
  uint64_t a = 11 / 0;
  dbgln("Recovered");

  while (1)
    ;
}
