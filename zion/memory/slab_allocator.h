#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_counted.h>
#include <glacier/status/error.h>
#include <glacier/status/error_or.h>
#include <stdint.h>

#include "memory/paging_util.h"

class Slab : public glcr::RefCounted<Slab>,
             public glcr::IntrusiveListNode<Slab> {
 public:
  explicit Slab(uint64_t elem_size);
  Slab(Slab&) = delete;
  Slab(Slab&&) = delete;

  void* Allocate();
  void Free(void* addr);

  bool IsFull();
  uint64_t Allocations() { return num_allocated_; }

 private:
  struct FreeListEntry {
    FreeListEntry* next;
  };

  uint64_t elem_size_;
  uint64_t num_allocated_;
  FreeListEntry* first_free_;

  bool IsContained(void* addr);
};

class SlabAllocator {
 public:
  SlabAllocator(uint64_t elem_size) : elem_size_(elem_size) {}
  SlabAllocator(SlabAllocator&) = delete;

  glcr::ErrorOr<void*> Allocate();

  // Stats:
  uint64_t SlabCount();
  uint64_t Allocations();

 private:
  uint64_t elem_size_;
  glcr::IntrusiveList<Slab> slabs_;
};

void SlabFree(void* addr);
