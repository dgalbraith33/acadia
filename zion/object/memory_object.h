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

 protected:
  // Hacky to avoid linked_list creation.
  MemoryObject(uint64_t size, bool) : size_(size) {}

 private:
  // Always stores the full page-aligned size.
  uint64_t size_;

  virtual uint64_t PageNumberToPhysAddr(uint64_t page_num);

  LinkedList<uint64_t> phys_page_list_;
};

class FixedMemoryObject : public MemoryObject {
 public:
  // FIXME: Validate that this is 4k aligned.
  FixedMemoryObject(uint64_t physical_addr, uint64_t size)
      : MemoryObject(size, true), physical_addr_(physical_addr) {}

 private:
  uint64_t physical_addr_;

  uint64_t PageNumberToPhysAddr(uint64_t page_num) override {
    return physical_addr_ + (0x1000 * page_num);
  }
};
