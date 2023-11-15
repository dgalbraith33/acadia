#pragma once

#include <glacier/memory/unique_ptr.h>
#include <stdint.h>

#include "memory/slab_allocator.h"

class KernelHeap {
 public:
  KernelHeap(uint64_t lower_bound, uint64_t upper_bound);
  void InitializeSlabAllocators();

  void* Allocate(uint64_t size);

  void Free(void* address);

  static void DumpDistribution();

 private:
  uint64_t next_addr_;
  uint64_t upper_bound_;

  glcr::UniquePtr<SlabAllocator> slab_8_;
  glcr::UniquePtr<SlabAllocator> slab_16_;
  glcr::UniquePtr<SlabAllocator> slab_32_;

  // Distribution collection for the purpose of investigating a slab allocator.
  // 0: 0-4B
  // 1: 4B-8B
  // 2: 8B-16B
  // 3: 16B-32B
  // 4: 32B-64B
  // 5: 64B-128B
  // 6: 128B-256B
  // 7: 256B-512B
  // 8: 512B-1KiB
  // 9: 1KiB-2KiB
  // 10: 2KiB-4KiB
  // 11: 4KiB+
  uint64_t distributions[12];

  void RecordSize(uint64_t size);
};
