#pragma once

#include "lib/linked_list.h"
#include "object/kernel_object.h"

/*
 * MemoryObject is a page-aligned set of memory that corresponds
 * to physical pages.
 *
 * It can be mapped in to one or more address spaces.
 * */
class MemoryObject : public KernelObject {
 public:
  MemoryObject(uint64_t size);

  uint64_t size() { return size_; }
  uint64_t num_pages() { return size_ / 0x1000; }

  uint64_t PhysicalPageAtOffset(uint64_t offset);

  void CopyBytesToObject(uint64_t source, uint64_t length);

 private:
  // Always stores the full page-aligned size.
  uint64_t size_;

  uint64_t PageNumberToPhysAddr(uint64_t page_num);

  LinkedList<uint64_t> phys_page_list_;
};
