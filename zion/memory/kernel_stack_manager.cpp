#include "memory/kernel_stack_manager.h"

#include "common/gdt.h"
#include "debug/debug.h"
#include "memory/paging_util.h"

#define KERNEL_STACK_START 0xFFFFFFFF'90000000
#define KERNEL_STACK_LIMIT 0xFFFFFFFF'9FFFFFFF
#define KERNEL_STACK_OFFSET 0x4000

KernelStackManager* gKernelStackManager;

void KernelStackManager::Init() {
  gKernelStackManager = new KernelStackManager();

  SetIst1(gKernelStackManager->AllocateKernelStack());
}

KernelStackManager::KernelStackManager()
    : next_stack_addr_(KERNEL_STACK_START) {}

uint64_t* KernelStackManager::AllocateKernelStack() {
  next_stack_addr_ += KERNEL_STACK_OFFSET;
  if (next_stack_addr_ >= KERNEL_STACK_LIMIT) {
    panic("No more kernelstack space");
  }
  EnsureResident(next_stack_addr_ - 0x3000, 0x3000);
  return reinterpret_cast<uint64_t*>(next_stack_addr_) - 1;
}

void KernelStackManager::FreeKernelStack(uint64_t stack_base) {
  freed_stack_cnt_++;
  dbgln("Freed kernel stacks using {} KiB", freed_stack_cnt_ * 12);
}
