#include <stdint.h>

#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"
#include "interrupt/timer.h"
#include "loader/init_loader.h"
#include "memory/kernel_heap.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

extern "C" void zion() {
  dbgln("[boot] Init GDT & IDT.");
  InitGdt();
  InitIdt();

  dbgln("[boot] Init Paging.");
  InitPaging();

  dbgln("[boot] Init Physical Memory Manager.");
  phys_mem::InitBootstrapPageAllocation();
  KernelHeap heap(0xFFFFFFFF'40000000, 0xFFFFFFFF'80000000);
  phys_mem::InitPhysicalMemoryManager();

  dbgln("[boot] Init syscalls.");
  InitSyscall();

  dbgln("[boot] Init scheduler.");
  // Schedule every 50ms.
  SetFrequency(/* hertz= */ 20);
  sched::InitScheduler();

  dbgln("[boot] Loading sys init program.");
  LoadInitProgram();

  dbgln("[boot] Init finished, yielding.");
  sched::EnableScheduler();
  sched::Yield();

  dbgln("Sleeping!");
  while (1)
    ;
}
