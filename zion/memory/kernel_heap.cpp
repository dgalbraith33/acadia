#include "memory/kernel_heap.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

#define K_HEAP_DEBUG 1

namespace {

static KernelHeap* gKernelHeap = nullptr;

KernelHeap& GetKernelHeap() {
  if (!gKernelHeap) {
    panic("Kernel Heap not initialized.");
  }
  return *gKernelHeap;
}

static uint8_t kNewByte = 0xAB;

void InitMemory(uint8_t* addr, uint64_t size) {
  for (uint64_t i = 0; i < size; i++) {
    addr[i] = kNewByte;
  }
}

bool IsSlab(void* addr) {
  uint64_t uaddr = reinterpret_cast<uint64_t>(addr);

  return uaddr >= kKernelSlabHeapStart && uaddr < kKernelSlabHeapEnd;
}

}  // namespace

KernelHeap::KernelHeap() { gKernelHeap = this; }

void* KernelHeap::Allocate(uint64_t size) {
  if (size <= 8) {
    auto ptr_or = slab_8_.Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 8): {x}", ptr_or.error());
#endif
  }
  if (size <= 16) {
    auto ptr_or = slab_16_.Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 16): {x}", ptr_or.error());
#endif
  }
  if (size <= 32) {
    auto ptr_or = slab_32_.Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 32): {x}", ptr_or.error());
#endif
  }
  if (size <= 64) {
    auto ptr_or = slab_64_.Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 64): {x}", ptr_or.error());
#endif
  }
  if (size <= 128) {
    auto ptr_or = slab_128_.Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 128): {x}", ptr_or.error());
#endif
  }
  if (next_addr_ + size >= upper_bound_) {
    panic("Kernel Heap Overrun (next, size, max): {x}, {x}, {x}", next_addr_,
          size, upper_bound_);
  }
#if K_HEAP_DEBUG
  RecordSize(size);
#endif
  EnsureResident(next_addr_, size);
  uint64_t address = next_addr_;
  alloc_count_ += 1;
  next_addr_ += size;
  return reinterpret_cast<void*>(address);
}

void KernelHeap::DumpDebugData() {
#if K_HEAP_DEBUG
  gKernelHeap->DumpDebugDataInternal();
#endif
}

void KernelHeap::DumpDebugDataInternal() {
  dbgln("");
  dbgln("Heap Debug Statistics!");

  dbgln("Pages used: {}",
        (next_addr_ - kKernelBuddyHeapStart - 1) / 0x1000 + 1);
  // Active Allocs.
  dbgln("Active Allocations: {}", alloc_count_);

  dbgln("Slab Statistics:");
  dbgln("Slab 8: {} slabs, {} allocs", slab_8_.SlabCount(),
        slab_8_.Allocations());
  dbgln("Slab 16: {} slabs, {} allocs", slab_16_.SlabCount(),
        slab_16_.Allocations());
  dbgln("Slab 32: {} slabs, {} allocs", slab_32_.SlabCount(),
        slab_32_.Allocations());
  dbgln("Slab 64: {} slabs, {} allocs", slab_64_.SlabCount(),
        slab_64_.Allocations());
  dbgln("Slab 128: {} slabs, {} allocs", slab_128_.SlabCount(),
        slab_128_.Allocations());

  dbgln("");
  dbgln("Size Distributions of non slab-allocated.");
  dbgln("<=8B:   {}", distributions[0]);
  dbgln("<=16B:  {}", distributions[1]);
  dbgln("<=32B:  {}", distributions[2]);
  dbgln("<=64B:  {}", distributions[3]);
  dbgln("<=128B: {}", distributions[4]);
  dbgln("<=256B: {}", distributions[5]);
  dbgln("<=512B: {}", distributions[6]);
  dbgln("<=1KiB: {}", distributions[7]);
  dbgln("<=2KiB: {}", distributions[8]);
  dbgln("<=4KiB: {}", distributions[9]);
  dbgln("> 4KiB: {}", distributions[10]);
  dbgln("");
}

void KernelHeap::RecordSize(uint64_t size) {
  size -= 1;
  size >>= 3;
  uint64_t index = 0;
  while (size && index < 11) {
    size >>= 1;
    index++;
  }
  distributions[index]++;
}

void* operator new(uint64_t size) {
  void* addr = GetKernelHeap().Allocate(size);
  InitMemory(static_cast<uint8_t*>(addr), size);
  return addr;
}
void* operator new[](uint64_t size) {
  void* addr = GetKernelHeap().Allocate(size);
  InitMemory(static_cast<uint8_t*>(addr), size);
  return addr;
}

void operator delete(void* addr, uint64_t size) {
  if (IsSlab(addr)) {
    SlabFree(addr);
  }
}
void operator delete[](void* addr) {
  if (IsSlab(addr)) {
    SlabFree(addr);
  }
}
