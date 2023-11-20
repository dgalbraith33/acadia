#include "object/memory_object.h"

#include "boot/boot_info.h"
#include "debug/debug.h"
#include "memory/physical_memory.h"

#define K_MEM_DEBUG 0

uint64_t MemoryObject::PhysicalPageAtOffset(uint64_t offset) {
  if (offset > size()) {
    panic("Invalid offset");
  }
  uint64_t page_num = offset / 0x1000;
  return PageNumberToPhysAddr(page_num);
}

void MemoryObject::CopyBytesToObject(uint64_t source, uint64_t length) {
  if (length > size()) {
    panic("Copy overruns memory object: {x} too large for {x}", length, size());
  }
  uint64_t hhdm = boot::GetHigherHalfDirectMap();
  uint64_t page_number = 0;
  while (length > 0) {
    uint64_t physical = hhdm + PageNumberToPhysAddr(page_number);

    uint64_t bytes_to_copy = length >= 0x1000 ? 0x1000 : length;

    uint8_t* srcptr = reinterpret_cast<uint8_t*>(source);
    uint8_t* destptr = reinterpret_cast<uint8_t*>(physical);
    for (uint64_t i = 0; i < bytes_to_copy; i++) {
      destptr[i] = srcptr[i];
    }

    length -= bytes_to_copy;
    source += 0x1000;
    page_number++;
  }
}

VariableMemoryObject::VariableMemoryObject(uint64_t size) : size_(size) {
  if ((size & 0xFFF) != 0) {
    size_ = (size & ~0xFFF) + 0x1000;
#if K_MEM_DEBUG
    dbgln("MemoryObject: aligned {x} to {x}", size, size_);
#endif
  }
  // FIXME: Do this lazily.
  uint64_t num_pages = size_ / 0x1000;
  phys_page_list_ = glcr::Array<uint64_t>(num_pages);
  for (uint64_t i = 0; i < phys_page_list_.size(); i++) {
    phys_page_list_[i] = 0;
  }
}

VariableMemoryObject::~VariableMemoryObject() {
  for (uint64_t p = 0; p < phys_page_list_.size(); p++) {
    if (phys_page_list_[p] != 0) {
      // TODO: We may be able to do some sort of coalescing here.
      phys_mem::FreePage(phys_page_list_[p]);
    }
  }
}

uint64_t VariableMemoryObject::PageNumberToPhysAddr(uint64_t page_num) {
  if (phys_page_list_[page_num] == 0) {
#if K_MEM_DEBUG
    dbgln("Allocating page num {} for mem object", page_num);
#endif
    phys_page_list_[page_num] = phys_mem::AllocatePage();
  }
  return phys_page_list_[page_num];
}

FixedMemoryObject::~FixedMemoryObject() {
  if (should_free_) {
    phys_mem::FreePages(physical_addr_, num_pages());
  }
}

glcr::ErrorOr<glcr::RefPtr<MemoryObject>> FixedMemoryObject::Duplicate(
    uint64_t offset, uint64_t length) {
  if (offset + length > size()) {
    return glcr::INVALID_ARGUMENT;
  }

  return glcr::StaticCastRefPtr<MemoryObject>(
      glcr::MakeRefCounted<FixedMemoryObject>(physical_addr_ + offset, length,
                                              false));
}
