#include <stdint.h>

#include "common/gdt.h"
#include "debug/debug.h"

extern "C" void zion() {
  dbgln("Hello World!");
  InitGdt();
  dbgln("New GDT Loaded!");

  while (1)
    ;
}
