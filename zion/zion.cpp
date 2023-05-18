#include <stdint.h>

#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"
#include "memory/kernel_heap.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

extern "C" void zion() {
  InitGdt();
  InitIdt();
  InitPaging();

  phys_mem::InitBootstrapPageAllocation();
  KernelHeap heap(0xFFFFFFFF'40000000, 0xFFFFFFFF'80000000);
  phys_mem::InitPhysicalMemoryManager();

  InitSyscall();

  sched::InitScheduler();
  Process p1;
  p1.CreateThread();
  Process p2;
  p2.CreateThread();
  sched::EnableScheduler();
  sched::Yield();

  dbgln("Sleeping!");
  while (1)
    ;
}
