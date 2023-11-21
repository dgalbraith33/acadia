#include <stdint.h>

#include "boot/acpi.h"
#include "common/cpu.h"
#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/apic.h"
#include "interrupt/apic_timer.h"
#include "interrupt/interrupt.h"
#include "interrupt/timer.h"
#include "loader/init_loader.h"
#include "memory/kernel_heap.h"
#include "memory/kernel_stack_manager.h"
#include "memory/kernel_vmm.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

extern "C" void zion() {
  dbgln("[boot] Init GDT & IDT.");
  InitGdt();
  InitIdt();

  ProbeCpuAndEnableFeatures();

  dbgln("[boot] Init Physical Memory Manager.");
  phys_mem::InitBootstrapPageAllocation();

  // - Must happen after BootstrapPageAllocation
  // due to the heap using page allocations.
  // - Must happen after InitIdt() as the kernel
  // stack manager will update ist1.
  KernelVmm kvmm;

  // Must happen after KernelVmm init as it
  // will do allocations to build the free list.
  phys_mem::InitPhysicalMemoryManager();

  dbgln("[boot] Memory allocations available now.");

  dbgln("[boot] Probing Hardware");
  ProbeRsdp();

  // These two need to occur after memory allocation is available.
  Apic::Init();
  ApicTimer::Init();

  dbgln("[boot] Init syscalls.");
  InitSyscall();

  dbgln("[boot] Init scheduler.");
  ProcessManager::Init();
  Scheduler::Init();

  dbgln("[boot] Loading sys init program.");
  LoadInitProgram();

  dbgln("[boot] Init finished, yielding.");
  gScheduler->Enable();
  gScheduler->Yield();

  while (1)
    ;
}
