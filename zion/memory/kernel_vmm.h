#pragma once

#include <stdint.h>

#include "memory/constants.h"

class KernelVmm {
 public:
  KernelVmm();

  static uint64_t AcquireSlabHeapRegion(uint64_t slab_size_bytes);

 private:
  uint64_t next_slab_heap_page_ = kKernelSlabHeapStart;

  uint64_t AcquireSlabHeapRegionInternal(uint64_t slab_size_bytes);
};
