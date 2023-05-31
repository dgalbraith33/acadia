#include "memory/virtual_memory.h"

#include "memory/kernel_stack_manager.h"

extern KernelStackManager* gKernelStackManager;

uint64_t* VirtualMemory::AllocateKernelStack() {
  return gKernelStackManager->AllocateKernelStack();
}

uint64_t VirtualMemory::GetNextMemMapAddr(uint64_t size) {
  uint64_t addr = next_memmap_addr_;
  next_memmap_addr_ += size;
  if (next_memmap_addr_ >= 0x30'00000000) {
    panic("OOM: Memmap");
  }
  return addr;
}
