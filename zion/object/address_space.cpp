#include "object/address_space.h"

#include "debug/debug.h"
#include "memory/kernel_vmm.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"

#define K_VMAS_DEBUG 0

glcr::RefPtr<AddressSpace> AddressSpace::ForRoot() {
  uint64_t cr3 = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(cr3));
  return glcr::MakeRefCounted<AddressSpace>(cr3);
}

AddressSpace::AddressSpace() {
  cr3_ = phys_mem::AllocateAndZeroPage();
  InitializePml4(cr3_);
}

AddressSpace::~AddressSpace() { CleanupPml4(cr3_); }

glcr::ErrorOr<uint64_t> AddressSpace::AllocateUserStack() {
  uint64_t base = user_stacks_.NewUserStack();
  auto mem_object = glcr::StaticCastRefPtr<MemoryObject>(
      glcr::MakeRefCounted<VariableMemoryObject>(kUserStackSize));
  RET_ERR(MapInMemoryObject(base, mem_object));
  return base;
}

glcr::ErrorCode AddressSpace::FreeUserStack(uint64_t base) {
  RET_ERR(FreeAddressRange(base, base + kUserStackSize));
  user_stacks_.FreeUserStack(base);
  return glcr::OK;
}

uint64_t AddressSpace::GetNextMemMapAddr(uint64_t size, uint64_t align) {
  if (size == 0) {
    panic("Zero size memmap");
  }
  size = ((size - 1) & ~0xFFF) + 0x1000;
  // FIXME: We need to validate that align is a power of 2;
  if (align > 0) {
    while ((next_memmap_addr_ & (align - 1)) != 0) {
      next_memmap_addr_ += kPageSize;
    }
  }
  uint64_t addr = next_memmap_addr_;
  next_memmap_addr_ += size;
  if (next_memmap_addr_ >= 0x30'00000000) {
    panic("OOM: Memmap");
  }
  return addr;
}

glcr::ErrorCode AddressSpace::MapInMemoryObject(
    uint64_t vaddr, const glcr::RefPtr<MemoryObject>& mem_obj) {
  return mapping_tree_.AddInMemoryObject(vaddr, mem_obj);
}

glcr::ErrorOr<uint64_t> AddressSpace::MapInMemoryObject(
    const glcr::RefPtr<MemoryObject>& mem_obj, uint64_t align) {
  uint64_t vaddr = GetNextMemMapAddr(mem_obj->size(), align);
  RET_ERR(mapping_tree_.AddInMemoryObject(vaddr, mem_obj));
  return vaddr;
}

uint64_t AddressSpace::AllocateKernelStack() {
  return KernelVmm::AcquireKernelStack();
}

bool AddressSpace::HandlePageFault(uint64_t vaddr) {
#if K_VMAS_DEBUG
  dbgln("[VMAS] Page Fault!");
#endif
  if (vaddr < kPageSize) {
    // Invalid page access.
    return false;
  }

  auto offset_or = mapping_tree_.GetPhysicalPageAtVaddr(vaddr);
  if (!offset_or.ok()) {
    return false;
  }
#if K_VMAS_DEBUG
  dbgln("[VMAS] Mapping P({x}) at V({x})", physical_addr, vaddr);
#endif
  MapPage(cr3_, vaddr, offset_or.value());
  return true;
}
