#include "memory/kernel_heap.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

#define K_HEAP_DEBUG 0

namespace {

static KernelHeap* gKernelHeap = nullptr;

KernelHeap& GetKernelHeap() {
  if (!gKernelHeap) {
    panic("Kernel Heap not initialized.");
  }
  return *gKernelHeap;
}
}  // namespace

KernelHeap::KernelHeap(uint64_t lower_bound, uint64_t upper_bound)
    : next_addr_(lower_bound), upper_bound_(upper_bound) {
  gKernelHeap = this;
}

void* KernelHeap::Allocate(uint64_t size) {
  if (next_addr_ + size >= upper_bound_) {
    panic("Kernel Heap Overrun (next, size, max): %m, %x, %m", next_addr_, size,
          upper_bound_);
  }
#if K_HEAP_DEBUG
  RecordSize(size);
#endif
  EnsureResident(next_addr_, size);
  uint64_t address = next_addr_;
  next_addr_ += size;
  return reinterpret_cast<void*>(address);
}

void KernelHeap::DumpDistribution() {
#if K_HEAP_DEBUG
  uint64_t* distributions = gKernelHeap->distributions;
  dbgln("<=4B:   %u", distributions[0]);
  dbgln("<=8B:   %u", distributions[1]);
  dbgln("<=16B:  %u", distributions[2]);
  dbgln("<=32B:  %u", distributions[3]);
  dbgln("<=64B:  %u", distributions[4]);
  dbgln("<=128B: %u", distributions[5]);
  dbgln("<=256B: %u", distributions[6]);
  dbgln("<=512B: %u", distributions[7]);
  dbgln("<=1KiB: %u", distributions[8]);
  dbgln("<=2KiB: %u", distributions[9]);
  dbgln("<=4KiB: %u", distributions[10]);
  dbgln("> 4KiB: %u", distributions[11]);
#endif
}

void KernelHeap::RecordSize(uint64_t size) {
  size >>= 3;
  uint64_t index = 0;
  while (size && index < 11) {
    size >>= 1;
    index++;
  }
  distributions[index]++;
}

void* operator new(uint64_t size) { return GetKernelHeap().Allocate(size); }
void* operator new[](uint64_t size) { return GetKernelHeap().Allocate(size); }

void operator delete(void*, uint64_t) {}
void operator delete[](void*) {}
