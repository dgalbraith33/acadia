#include <stdint.h>

#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"
#include "memory/kernel_heap.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"

extern "C" void zion() {
  InitGdt();
  InitIdt();
  InitPaging();

  phys_mem::InitBootstrapPageAllocation();
  KernelHeap heap(0xFFFFFFFF'40000000, 0xFFFFFFFF'80000000);
  phys_mem::InitPhysicalMemoryManager();
  heap.Allocate(0x2000);

  dbgln("Sleeping!");
  while (1)
    ;
}
