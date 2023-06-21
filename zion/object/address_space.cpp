#include "object/address_space.h"

#include "memory/kernel_stack_manager.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"

#define K_VMAS_DEBUG 0

extern KernelStackManager* gKernelStackManager;

glcr::RefPtr<AddressSpace> AddressSpace::ForRoot() {
  uint64_t cr3 = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(cr3));
  return glcr::MakeRefCounted<AddressSpace>(cr3);
}

AddressSpace::AddressSpace() {
  cr3_ = phys_mem::AllocatePage();
  InitializePml4(cr3_);
}

uint64_t AddressSpace::AllocateUserStack() {
  return user_stacks_.NewUserStack();
}

uint64_t AddressSpace::GetNextMemMapAddr(uint64_t size) {
  if (size == 0) {
    panic("Zero size memmap");
  }
  size = ((size - 1) & ~0xFFF) + 0x1000;
  uint64_t addr = next_memmap_addr_;
  next_memmap_addr_ += size;
  if (next_memmap_addr_ >= 0x30'00000000) {
    panic("OOM: Memmap");
  }
  return addr;
}

void AddressSpace::MapInMemoryObject(
    uint64_t vaddr, const glcr::RefPtr<MemoryObject>& mem_obj) {
  memory_mappings_.PushBack({.vaddr = vaddr, .mem_obj = mem_obj});
}

uint64_t AddressSpace::MapInMemoryObject(
    const glcr::RefPtr<MemoryObject>& mem_obj) {
  uint64_t vaddr = GetNextMemMapAddr(mem_obj->size());
  memory_mappings_.PushBack({.vaddr = vaddr, .mem_obj = mem_obj});
  return vaddr;
}

uint64_t* AddressSpace::AllocateKernelStack() {
  return gKernelStackManager->AllocateKernelStack();
}

bool AddressSpace::HandlePageFault(uint64_t vaddr) {
#if K_VMAS_DEBUG
  dbgln("[VMAS] Page Fault!");
#endif
  if (user_stacks_.IsValidStack(vaddr)) {
    MapPage(cr3_, vaddr, phys_mem::AllocatePage());
    return true;
  }

  MemoryMapping* mapping = GetMemoryMappingForAddr(vaddr);
  if (mapping == nullptr) {
    return false;
  }
  uint64_t offset = vaddr - mapping->vaddr;
  uint64_t physical_addr = mapping->mem_obj->PhysicalPageAtOffset(offset);
  if (physical_addr == 0) {
    dbgln("WARN: Memory object returned invalid physical addr.");
    return false;
  }
#if K_VMAS_DEBUG
  dbgln("[VMAS] Mapping P(%m) at V(%m)", physical_addr, vaddr);
#endif
  MapPage(cr3_, vaddr, physical_addr);
  return true;
}

AddressSpace::MemoryMapping* AddressSpace::GetMemoryMappingForAddr(
    uint64_t vaddr) {
  auto iter = memory_mappings_.begin();
  while (iter != memory_mappings_.end()) {
    if ((vaddr >= (*iter).vaddr) &&
        (vaddr < ((*iter).vaddr + (*iter).mem_obj->size()))) {
      return &(*iter);
    }
    ++iter;
  }

  return 0;
}
