#include "memory/kernel_stack_manager.h"

#include "common/gdt.h"
#include "debug/debug.h"
#include "interrupt/interrupt.h"
#include "memory/constants.h"
#include "memory/paging_util.h"

KernelStackManager::KernelStackManager()
    : next_stack_addr_(kKernelStackStart) {}

void KernelStackManager::SetupInterruptStack() {
  SetIst1(AllocateKernelStack());
  UpdateFaultHandlersToIst1();
}

uint64_t KernelStackManager::AllocateKernelStack() {
  if (!free_stacks_.empty()) {
    return free_stacks_.PopFront();
  }
  next_stack_addr_ += kKernelStackOffset;
  if (next_stack_addr_ >= kKernelStackEnd) {
    panic("No more kernel stack space");
  }
  EnsureResident(next_stack_addr_ - kKernelStackSize, kKernelStackSize);
  return next_stack_addr_ - 8;
}

void KernelStackManager::FreeKernelStack(uint64_t stack_base) {
  // TODO: Validate this value.
  if ((stack_base & 0x3FF8) != 0x3FF8) {
    dbgln("Odd kernel stack free {x}", stack_base);
  }
  free_stacks_.PushFront(stack_base);
}
