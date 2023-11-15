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

  glcr::UniquePtr<SlabAllocator> slab_8_;
  glcr::UniquePtr<SlabAllocator> slab_16_;
  glcr::UniquePtr<SlabAllocator> slab_32_;

  // Distribution collection for the purpose of investigating a slab allocator.
  // 0: 0-8B
  // 1: 8B-16B
  // 2: 16B-32B
  // 3: 32B-64B
  // 4: 64B-128B
  // 5: 128B-256B
  // 6: 256B-512B
  // 7: 512B-1KiB
  // 8: 1KiB-2KiB
  // 9: 2KiB-4KiB
  // 10: 4KiB+
  uint64_t distributions[11];

  void RecordSize(uint64_t size);

  void DumpDebugDataInternal();
};
