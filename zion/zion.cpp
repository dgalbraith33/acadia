#include <stdint.h>

#include "boot/boot_info.h"
#include "boot/limine.h"
#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"
#include "memory/kernel_heap.h"
#include "memory/paging_util.h"

extern "C" void zion() {
  InitGdt();
  InitIdt();
  InitPaging();

  KernelHeap heap(0xFFFFFFFF'40000000, 0xFFFFFFFF'80000000);
  heap.Allocate(1);

  while (1)
    ;
}
