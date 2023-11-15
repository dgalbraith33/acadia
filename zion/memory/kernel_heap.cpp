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

static uint8_t kNewByte = 0xAB;

void InitMemory(uint8_t* addr, uint64_t size) {
  for (uint64_t i = 0; i < size; i++) {
    addr[i] = kNewByte;
  }
}

}  // namespace

KernelHeap::KernelHeap(uint64_t lower_bound, uint64_t upper_bound)
    : next_addr_(lower_bound), upper_bound_(upper_bound) {
  gKernelHeap = this;
}

void KernelHeap::InitializeSlabAllocators() {
  slab_8_ = glcr::MakeUnique<SlabAllocator>(8);
  slab_16_ = glcr::MakeUnique<SlabAllocator>(16);
  slab_32_ = glcr::MakeUnique<SlabAllocator>(32);
}

void* KernelHeap::Allocate(uint64_t size) {
#if K_HEAP_DEBUG
  dbgln("Alloc ({x})", size);
#endif
  if ((size <= 8) && slab_8_) {
    auto ptr_or = slab_8_->Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 8): {x}", ptr_or.error());
#endif
  }
  if ((size <= 16) && slab_16_) {
    auto ptr_or = slab_16_->Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 16): {x}", ptr_or.error());
#endif
  }
  if ((size <= 32) && slab_32_) {
    auto ptr_or = slab_32_->Allocate();
    if (ptr_or.ok()) {
      return ptr_or.value();
    }
#if K_HEAP_DEBUG
    dbgln("Skipped allocation (slab 32): {x}", ptr_or.error());
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
  next_addr_ += size;
  return reinterpret_cast<void*>(address);
}

void KernelHeap::DumpDistribution() {
#if K_HEAP_DEBUG
  uint64_t* distributions = gKernelHeap->distributions;
  dbgln("<=4B:   {}", distributions[0]);
  dbgln("<=8B:   {}", distributions[1]);
  dbgln("<=16B:  {}", distributions[2]);
  dbgln("<=32B:  {}", distributions[3]);
  dbgln("<=64B:  {}", distributions[4]);
  dbgln("<=128B: {}", distributions[5]);
  dbgln("<=256B: {}", distributions[6]);
  dbgln("<=512B: {}", distributions[7]);
  dbgln("<=1KiB: {}", distributions[8]);
  dbgln("<=2KiB: {}", distributions[9]);
  dbgln("<=4KiB: {}", distributions[10]);
  dbgln("> 4KiB: {}", distributions[11]);
#endif
}

void KernelHeap::RecordSize(uint64_t size) {
  size -= 1;
  size >>= 2;
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

void operator delete(void*, uint64_t size) {}
void operator delete[](void*) {}
