#pragma once

#include <stdint.h>

#include "debug/debug.h"

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
// 0x00007FFF 00000000 - 0x00007FFF FFFFFFFF : USER_STACK (64 GiB)
//
// Kernel Regions (Shared across processes):
// 0xFFFF8000 00000000 - 0xFFFF800F FFFFFFFF : HHDM (64 GiB)
// 0xFFFFFFFF 40000000 - 0xFFFFFFFF 7FFFFFFF : KERNEL_HEAP (1 GiB)
// 0xFFFFFFFF 80000000 - 0xFFFFFFFF 80FFFFFF : KERNEL_CODE (16 MiB)
// 0xFFFFFFFF 90000000 - 0xFFFFFFFF 9FFFFFFF : KERNEL_STACK (256 MiB)
class VirtualMemory {
 public:
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

  static VirtualMemory ForRoot();

  VirtualMemory();
  VirtualMemory(const VirtualMemory&) = delete;
  VirtualMemory(VirtualMemory&&) = delete;

  uint64_t cr3() { return cr3_; }

  uint64_t GetNextMemMapAddr(uint64_t size);

  // Kernel
  uint64_t* AllocateKernelStack();

 private:
  VirtualMemory(uint64_t cr3) : cr3_(cr3) {}
  uint64_t cr3_ = 0;

  uint64_t next_memmap_addr_ = 0x20'00000000;
};
