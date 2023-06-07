#include "object/address_space.h"

#include "memory/kernel_stack_manager.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"

extern KernelStackManager* gKernelStackManager;

AddressSpace AddressSpace::ForRoot() {
  uint64_t cr3 = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(cr3));
  return {cr3};
}

AddressSpace::AddressSpace() {
  cr3_ = phys_mem::AllocatePage();
  InitializePml4(cr3_);
}

uint64_t AddressSpace::AllocateUserStack() {
  return user_stacks_.NewUserStack();
}

uint64_t AddressSpace::GetNextMemMapAddr(uint64_t size) {
  uint64_t addr = next_memmap_addr_;
  next_memmap_addr_ += size;
  if (next_memmap_addr_ >= 0x30'00000000) {
    panic("OOM: Memmap");
  }
  return addr;
}

uint64_t* AddressSpace::AllocateKernelStack() {
  return gKernelStackManager->AllocateKernelStack();
}
