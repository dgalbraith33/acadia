#pragma once

#include <stdint.h>

#include "memory/constants.h"
#include "memory/kernel_heap.h"
#include "memory/kernel_stack_manager.h"

class KernelVmm {
 public:
  KernelVmm();

  // TODO: Create a "MemoryRegion" style class to hold the return
  // types from this object.
  static uint64_t AcquireSlabHeapRegion(uint64_t slab_size_bytes);

  static uint64_t AcquireKernelStack();

  // Takes the base address to the stack. I.e. the highest value in it.
  static void FreeKernelStack(uint64_t);

 private:
  uint64_t next_slab_heap_page_ = kKernelSlabHeapStart;
  KernelHeap heap_;
  glcr::UniquePtr<KernelStackManager> stack_manager_;

  uint64_t AcquireSlabHeapRegionInternal(uint64_t slab_size_bytes);
};
