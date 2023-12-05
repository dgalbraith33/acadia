#pragma once

#include <glacier/container/binary_tree.h>
#include <glacier/memory/ref_ptr.h>
#include <stdint.h>

#include "include/ztypes.h"
#include "lib/memory_mapping_tree.h"
#include "memory/user_stack_manager.h"
#include "object/memory_object.h"

class AddressSpace;

template <>
struct KernelObjectTag<AddressSpace> {
  static const uint64_t type = KernelObject::ADDRESS_SPACE;
};

// VirtualMemory class holds a memory space for an individual process.
//
// Memory Regions are predefined for simplicity for now. However, in general
// we try not to rely on these regions being static to allow for flexibility in
// the future.
//
// User Regions (Per Process):
// 0x00000000 00000000 - 0x0000000F FFFFFFFF : USER_CODE (64 GiB)
// 0x00000010 00000000 - 0x0000001F FFFFFFFF : USER_HEAP (64 GiB)
// 0x00000020 00000000 - 0x0000002F FFFFFFFF : MEM_MAP (64 GiB)
// 0x00000040 00000000 - 0x0000004F FFFFFFFF : IPC_BUF (64 GiB)
// 0x00007FF0 00000000 - 0x00007FFF FFFFFFFF : USER_STACK (64 GiB)
//
// Kernel Regions (Shared across processes):
// 0xFFFF8000 00000000 - 0xFFFF800F FFFFFFFF : HHDM (64 GiB)
// 0xFFFFFFFF 40000000 - 0xFFFFFFFF 7FFFFFFF : KERNEL_HEAP (1 GiB)
// 0xFFFFFFFF 80000000 - 0xFFFFFFFF 80FFFFFF : KERNEL_CODE (16 MiB)
// 0xFFFFFFFF 90000000 - 0xFFFFFFFF 9FFFFFFF : KERNEL_STACK (256 MiB)
class AddressSpace : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::ADDRESS_SPACE; }

  static uint64_t DefaultPermissions() {
    return kZionPerm_Write | kZionPerm_Transmit;
  }

  enum MemoryType {
    UNSPECIFIED,
    UNMAPPED,
    USER_CODE,
    USER_HEAP,
    MEM_MAP,
    IPC_BUF,
    USER_STACK,
    HHDM,
    KERNEL_HEAP,
    KERNEL_CODE,
    KERNEL_STACK,
  };

  static glcr::RefPtr<AddressSpace> ForRoot();

  AddressSpace();
  AddressSpace(const AddressSpace&) = delete;
  AddressSpace(AddressSpace&&) = delete;

  // Deconstructing an address space will free all paging structures associated
  // with this address space.
  ~AddressSpace();

  uint64_t cr3() { return cr3_; }

  // User Mappings.
  glcr::ErrorOr<uint64_t> AllocateUserStack();
  [[nodiscard]] glcr::ErrorCode FreeUserStack(uint64_t stack_base);
  uint64_t GetNextMemMapAddr(uint64_t size, uint64_t align);

  // Maps in a memory object at a specific address.
  // Note this is unsafe for now as it may clobber other mappings.
  [[nodiscard]] glcr::ErrorCode MapInMemoryObject(
      uint64_t vaddr, const glcr::RefPtr<MemoryObject>& mem_obj);

  [[nodiscard]] glcr::ErrorOr<uint64_t> MapInMemoryObject(
      const glcr::RefPtr<MemoryObject>& mem_obj, uint64_t align);

  [[nodiscard]] glcr::ErrorCode FreeAddressRange(uint64_t vaddr_base,
                                                 uint64_t vaddr_limit);

  // Kernel Mappings.
  uint64_t AllocateKernelStack();

  // Returns true if the page fault has been resolved.
  [[nodiscard]] bool HandlePageFault(uint64_t vaddr);

 private:
  friend class glcr::MakeRefCountedFriend<AddressSpace>;
  AddressSpace(uint64_t cr3) : cr3_(cr3) {}
  uint64_t cr3_ = 0;

  UserStackManager user_stacks_;
  uint64_t next_memmap_addr_ = 0x20'00000000;

  MemoryMappingTree mapping_tree_;
};
