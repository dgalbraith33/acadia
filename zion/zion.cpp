#include <stdint.h>

#include "boot/boot_info.h"
#include "boot/limine.h"
#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"

extern "C" void zion() {
  InitGdt();
  InitIdt();

  const limine_memmap_response& resp = boot::GetMemoryMap();
  dbgln("Base,Length,Type");
  for (uint64_t i = 0; i < resp.entry_count; i++) {
    const limine_memmap_entry& entry = *resp.entries[i];
    dbgln("%m,%x,%u", entry.base, entry.length, entry.type);
  }

  while (1)
    ;
}
