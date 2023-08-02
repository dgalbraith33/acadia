#include <stdint.h>

#include "boot/acpi.h"
#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/apic.h"
#include "interrupt/apic_timer.h"
#include "interrupt/interrupt.h"
#include "interrupt/timer.h"
#include "loader/init_loader.h"
#include "memory/kernel_heap.h"
#include "memory/kernel_stack_manager.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

extern "C" void zion() {
  dbgln("[boot] Init GDT & IDT.");
  InitGdt();
  InitIdt();

  dbgln("[boot] Init Physical Memory Manager.");
  phys_mem::InitBootstrapPageAllocation();
  KernelHeap heap(0xFFFFFFFF'40000000, 0xFFFFFFFF'80000000);
  phys_mem::InitPhysicalMemoryManager();

  dbgln("[boot] Memory allocations available now.");

  dbgln("[boot] Probing Hardware");
  ProbeRsdp();

  // These two need to occur after memory allocation is available.
  Apic::Init();
  ApicTimer::Init();

  dbgln("[boot] Init Kernel Stack Manager.");
  KernelStackManager::Init();

  // The KernelStackManager sets Ist1 as a part of initialization so we can use
  // it now.
  UpdateFaultHandlersToIst1();

  dbgln("[boot] Init syscalls.");
  InitSyscall();

  dbgln("[boot] Init scheduler.");
  ProcessManager::Init();
  Scheduler::Init();

  dbgln("[boot] Loading sys init program.");
  LoadInitProgram();

  dbgln("[boot] Allocs during boot:");
  heap.DumpDistribution();

  dbgln("[boot] Init finished, yielding.");
  gScheduler->Enable();
  gScheduler->Yield();

  dbgln("Sleeping!");
  while (1)
    ;
}
