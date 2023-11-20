#pragma once

#include <glacier/container/array.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/status/error_or.h>

#include "include/ztypes.h"
#include "memory/constants.h"
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
    return kZionPerm_Write | kZionPerm_Read | kZionPerm_Duplicate |
           kZionPerm_Transmit;
  }

  MemoryObject() = default;
  virtual ~MemoryObject() = default;

  virtual uint64_t size() = 0;
  uint64_t num_pages() { return ((size() - 1) / kPageSize) + 1; }
  virtual glcr::ErrorOr<glcr::RefPtr<MemoryObject>> Duplicate(
      uint64_t offset, uint64_t length) = 0;

  uint64_t PhysicalPageAtOffset(uint64_t offset);
  void CopyBytesToObject(uint64_t source, uint64_t length);

 protected:
  virtual uint64_t PageNumberToPhysAddr(uint64_t page_num) = 0;
};

class VariableMemoryObject : public MemoryObject {
 public:
  explicit VariableMemoryObject(uint64_t size);
  ~VariableMemoryObject() override;

  uint64_t size() override { return size_; }

  virtual glcr::ErrorOr<glcr::RefPtr<MemoryObject>> Duplicate(
      uint64_t offset, uint64_t length) override {
    return glcr::UNIMPLEMENTED;
  }

 protected:
  virtual uint64_t PageNumberToPhysAddr(uint64_t page_num) override;

 private:
  // Always stores the full page-aligned size.
  uint64_t size_;

  glcr::Array<uint64_t> phys_page_list_;
};

class FixedMemoryObject : public MemoryObject {
 public:
  // FIXME: Validate that this is 4k aligned.
  // Create a new class object for should free.
  FixedMemoryObject(uint64_t physical_addr, uint64_t size, bool should_free)
      : size_(size), physical_addr_(physical_addr), should_free_(should_free) {}

  ~FixedMemoryObject() override;

  virtual uint64_t size() override { return size_; }
  virtual glcr::ErrorOr<glcr::RefPtr<MemoryObject>> Duplicate(
      uint64_t offset, uint64_t length) override;

 protected:
  uint64_t PageNumberToPhysAddr(uint64_t page_num) override {
    return physical_addr_ + (kPageSize * page_num);
  }

 private:
  uint64_t size_;
  uint64_t physical_addr_;
  bool should_free_;
};
