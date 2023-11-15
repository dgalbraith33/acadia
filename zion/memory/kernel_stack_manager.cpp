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
  next_stack_addr_ += kKernelStackOffset;
  if (next_stack_addr_ >= kKernelStackEnd) {
    panic("No more kernel stack space");
  }
  EnsureResident(next_stack_addr_ - kKernelStackSize, kKernelStackSize);
  return next_stack_addr_ - 8;
}

void KernelStackManager::FreeKernelStack(uint64_t stack_base) {
  freed_stack_cnt_++;
  dbgln("Freed kernel stacks using {} KiB", freed_stack_cnt_ * 12);
}
