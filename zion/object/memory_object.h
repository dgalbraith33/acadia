#pragma once

#include <glacier/container/linked_list.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/status/error_or.h>

#include "include/ztypes.h"
#include "object/kernel_object.h"

class MemoryObject;
template <>
struct KernelObjectTag<MemoryObject> {
  static const uint64_t type = KernelObject::MEMORY_OBJECT;
};

/*
 * MemoryObject is a page-aligned set of memory that corresponds
 * to physical pages.
 *
 * It can be mapped in to one or more address spaces.
 * */
class MemoryObject : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::MEMORY_OBJECT; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Write | kZionPerm_Read | kZionPerm_Duplicate;
  }

  MemoryObject(uint64_t size);

  uint64_t size() { return size_; }
  uint64_t num_pages() { return size_ / 0x1000; }

  uint64_t PhysicalPageAtOffset(uint64_t offset);

  void CopyBytesToObject(uint64_t source, uint64_t length);

  virtual glcr::ErrorOr<glcr::RefPtr<MemoryObject>> Duplicate(uint64_t offset,
                                                              uint64_t length) {
    return glcr::UNIMPLEMENTED;
  }

 protected:
  // Hacky to avoid linked_list creation.
  MemoryObject(uint64_t size, bool) : size_(size) {}

 private:
  // Always stores the full page-aligned size.
  uint64_t size_;

  virtual uint64_t PageNumberToPhysAddr(uint64_t page_num);

  glcr::LinkedList<uint64_t> phys_page_list_;
};

class FixedMemoryObject : public MemoryObject {
 public:
  // FIXME: Validate that this is 4k aligned.
  FixedMemoryObject(uint64_t physical_addr, uint64_t size)
      : MemoryObject(size, true), physical_addr_(physical_addr) {}

  virtual glcr::ErrorOr<glcr::RefPtr<MemoryObject>> Duplicate(
      uint64_t offset, uint64_t length) override;

 private:
  uint64_t physical_addr_;

  uint64_t PageNumberToPhysAddr(uint64_t page_num) override {
    return physical_addr_ + (0x1000 * page_num);
  }
};
