#include "memory/kernel_heap.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

KernelHeap::KernelHeap(uint64_t lower_bound, uint64_t upper_bound)
    : next_addr_(lower_bound), upper_bound_(upper_bound) {}

void* KernelHeap::Allocate(uint64_t size) {
  if (next_addr_ + size >= upper_bound_) {
    panic("Kernel Heap Overrun");
  }
  EnsureResident(next_addr_, size);
  uint64_t address = next_addr_;
  next_addr_ += size;
  return reinterpret_cast<void*>(address);
}
