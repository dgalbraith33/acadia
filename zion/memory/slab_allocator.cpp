#include "slab_allocator.h"

#include "debug/debug.h"
#include "memory/constants.h"
#include "memory/kernel_vmm.h"
#include "memory/paging_util.h"

namespace {

const uint64_t kSlabSize = 4 * KiB;
const uint64_t kSlabMask = ~(kSlabSize - 1);

}  // namespace

Slab::Slab(uint64_t elem_size) : elem_size_(elem_size), num_allocated_(0) {
  uint64_t first_elem_offset = ((sizeof(Slab) - 1) / elem_size_) + 1;
  uint64_t entry_addr =
      reinterpret_cast<uint64_t>(this) + first_elem_offset * elem_size_;
  FreeListEntry* entry = reinterpret_cast<FreeListEntry*>(entry_addr);
  first_free_ = entry;

  uint64_t max_entry = reinterpret_cast<uint64_t>(this) + kSlabSize;
  entry_addr += elem_size_;
  while (entry_addr < max_entry) {
    FreeListEntry* next_entry = reinterpret_cast<FreeListEntry*>(entry_addr);
    next_entry->next = nullptr;
    entry->next = next_entry;
    entry = next_entry;
    entry_addr += elem_size_;
  }
}

void* Slab::Allocate() {
  if (IsFull()) {
    panic("Allocating from full slab.");
  }

  FreeListEntry* to_return = first_free_;
  first_free_ = first_free_->next;
  num_allocated_++;
  return static_cast<void*>(to_return);
}

void Slab::Free(void* address) {
  if (!IsContained(address)) {
    panic("Freeing non-contained address from slab.");
  }

  uint64_t vaddr = reinterpret_cast<uint64_t>(address);
  if ((vaddr & ~(elem_size_ - 1)) != vaddr) {
    panic("Freeing non-aligned address");
  }

  FreeListEntry* new_free = static_cast<FreeListEntry*>(address);
  new_free->next = first_free_;
  first_free_ = new_free;
  num_allocated_--;
}

bool Slab::IsFull() { return first_free_ == nullptr; }
bool Slab::IsContained(void* addr) {
  uint64_t uaddr = reinterpret_cast<uint64_t>(addr);
  uint64_t uthis = reinterpret_cast<uint64_t>(this);
  bool is_above = uaddr > (uthis + sizeof(this));
  bool is_below = uaddr < (uthis + kSlabSize);
  return is_above && is_below;
}

// FIXME: Move all of these to a new file that can be included anywhere.
inline void* operator new(uint64_t, void* p) { return p; }

glcr::ErrorOr<void*> SlabAllocator::Allocate() {
  auto slab = slabs_.PeekFront();
  while (slab && slab->IsFull()) {
    slab = slab->next_;
  }

  if (slab) {
    return slab->Allocate();
  }

  void* next_slab =
      reinterpret_cast<void*>(KernelVmm::AcquireSlabHeapRegion(kSlabSize));
  slabs_.PushFront(glcr::AdoptPtr(new (next_slab) Slab(elem_size_)));
  return slabs_.PeekFront()->Allocate();
}

uint64_t SlabAllocator::SlabCount() {
  auto slab = slabs_.PeekFront();
  uint64_t count = 0;
  while (slab) {
    count++;
    slab = slab->next_;
  }
  return count;
}

uint64_t SlabAllocator::Allocations() {
  auto slab = slabs_.PeekFront();
  uint64_t count = 0;
  while (slab) {
    count += slab->Allocations();
    slab = slab->next_;
  }
  return count;
}

void SlabFree(void* addr) {
  Slab* slab =
      reinterpret_cast<Slab*>(reinterpret_cast<uint64_t>(addr) & kSlabMask);
  slab->Free(addr);
}
