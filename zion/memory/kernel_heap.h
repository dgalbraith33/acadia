#pragma once

#include <glacier/memory/unique_ptr.h>
#include <stdint.h>

#include "memory/constants.h"
#include "memory/slab_allocator.h"

class KernelHeap {
 public:
  KernelHeap();
  void InitializeSlabAllocators();

  void* Allocate(uint64_t size);

  void Free(void* address);

  static void DumpDebugData();

 private:
  uint64_t next_addr_ = kKernelBuddyHeapStart;
  uint64_t upper_bound_ = kKernelBuddyHeapEnd;

  uint64_t alloc_count_ = 0;

  SlabAllocator slab_8_{8};
  SlabAllocator slab_16_{16};
  SlabAllocator slab_32_{32};
  SlabAllocator slab_64_{64};
  SlabAllocator slab_128_{128};

  // 0: 128B-256B
  // 1: 256B-512B
  // 2: 512B-1KiB
  // 3: 1KiB-2KiB
  // 4: 2KiB-4KiB
  // 5: 4KiB+
  uint64_t distributions[6];

  void RecordSize(uint64_t size);

  void DumpDebugDataInternal();
};
