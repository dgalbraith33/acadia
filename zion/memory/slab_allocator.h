#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_counted.h>
#include <glacier/status/error.h>
#include <glacier/status/error_or.h>
#include <stdint.h>

#include "memory/paging_util.h"

constexpr uint64_t kSlabSentinel = 0xDEADBEEF'C0DEC0DE;

// TODO: Add variable sized slabs (i.e. multiple page slabs.)
template <uint64_t ElemSize>
class Slab : public glcr::RefCounted<Slab<ElemSize>>,
             public glcr::IntrusiveListNode<Slab<ElemSize>> {
 public:
  explicit Slab(uint64_t addr) : page_addr_(addr) {
    for (uint64_t i = 0; i < kBitmapLength; i++) {
      bitmap_[i] = 0;
    }

    bitmap_[0] = ElemSize <= 8 ? 0x3 : 0x1;
    EnsureResident(page_addr_, 16);
    uint64_t* first_elem = reinterpret_cast<uint64_t*>(page_addr_);
    first_elem[0] = kSlabSentinel;
    first_elem[1] = reinterpret_cast<uint64_t>(this);
  }
  Slab(Slab&) = delete;
  Slab(Slab&&) = delete;

  glcr::ErrorOr<void*> Allocate() {
    uint64_t index = GetFirstFreeIndex();
    if (index == 0) {
      return glcr::EXHAUSTED;
    }
    bitmap_[index / 64] |= (0x1 << (index % 64));
    return reinterpret_cast<void*>(page_addr_ + (index * ElemSize));
  }

  glcr::ErrorCode Free(void* addr) {
    uint64_t raw_addr = reinterpret_cast<uint64_t>(addr);
    if (raw_addr < page_addr_ || raw_addr > (page_addr_ + 0x1000)) {
      return glcr::INVALID_ARGUMENT;
    }
    // FIXME: Check alignment.
    uint64_t offset = raw_addr - page_addr_;
    uint64_t index = offset / ElemSize;
    if (index == 0) {
      return glcr::FAILED_PRECONDITION;
    }
    bitmap_[index / 64] &= ~(0x1 << (index % 64));
  }

  bool IsFull() {
    for (uint64_t i = 0; i < kBitmapLength; i++) {
      if (bitmap_[i] != -1) {
        return false;
      }
    }
    return true;
  }

 private:
  // FIXME: Likely a bug or two here if the number of elements doesn't evenly
  // divide in to the bitmap length.
  static constexpr uint64_t kBitmapLength = 0x1000 / ElemSize / 64;
  static constexpr uint64_t kMaxElements = 0x99E / ElemSize;
  uint64_t bitmap_[kBitmapLength];
  uint64_t page_addr_;

  uint64_t GetFirstFreeIndex() {
    uint64_t bi = 0;
    while (bi < kBitmapLength && (bitmap_[bi] == -1)) {
      bi++;
    }
    if (bi == kBitmapLength) {
      return 0;
    }
    // FIXME: Use hardware bit instructions here.
    uint64_t bo = 0;
    uint64_t bm = bitmap_[bi];
    while (bm & 0x1) {
      bm >>= 1;
      bo += 1;
    }
    return (bi * 64) + bo;
  }
};

template <uint64_t ElemSize>
class SlabAllocator {
 public:
  SlabAllocator() = delete;
  SlabAllocator(SlabAllocator&) = delete;

  // TODO: Add a Kernel VMMO Struct to hold things like this.
  SlabAllocator(uint64_t base_addr, uint64_t num_pages) {
    for (uint64_t i = 0; i < num_pages; i++, base_addr += 0x1000) {
      slabs_.PushBack(glcr::MakeRefCounted<Slab<ElemSize>>(base_addr));
    }
  }

  glcr::ErrorOr<void*> Allocate() {
    glcr::RefPtr<Slab<ElemSize>> curr = slabs_.PeekFront();
    while (curr) {
      if (curr->IsFull()) {
        curr = curr->next_;
        continue;
      }
      return curr->Allocate();
    }
    return glcr::EXHAUSTED;
  }

 private:
  glcr::IntrusiveList<Slab<ElemSize>> slabs_;
};
