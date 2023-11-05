#include "object/memory_object.h"

#include "boot/boot_info.h"
#include "debug/debug.h"
#include "memory/physical_memory.h"

#define K_MEM_DEBUG 0

MemoryObject::MemoryObject(uint64_t size) : size_(size) {
  if ((size & 0xFFF) != 0) {
    size_ = (size & ~0xFFF) + 0x1000;
#if K_MEM_DEBUG
    dbgln("MemoryObject: aligned {x} to {x}", size, size_);
#endif
  }
  // FIXME: Do this lazily.
  uint64_t num_pages = size_ / 0x1000;
  for (uint64_t i = 0; i < num_pages; i++) {
    phys_page_list_.PushBack(0);
  }
}

uint64_t MemoryObject::PhysicalPageAtOffset(uint64_t offset) {
  if (offset > size_) {
    panic("Invalid offset");
  }
  uint64_t page_num = offset / 0x1000;
  return PageNumberToPhysAddr(page_num);
}

void MemoryObject::CopyBytesToObject(uint64_t source, uint64_t length) {
  if (length > size_) {
    panic("Copy overruns memory object: {x} too large for {x}", length, size_);
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

uint64_t MemoryObject::PageNumberToPhysAddr(uint64_t page_num) {
  auto iter = phys_page_list_.begin();
  uint64_t index = 0;
  while (index < page_num) {
    ++iter;
    index++;
  }

  if (*iter == 0) {
#if K_MEM_DEBUG
    dbgln("Allocating page num {} for mem object", page_num);
#endif
    *iter = phys_mem::AllocatePage();
  }
  return *iter;
}

glcr::ErrorOr<glcr::RefPtr<MemoryObject>> FixedMemoryObject::Duplicate(
    uint64_t offset, uint64_t length) {
  if (offset + length > size()) {
    return glcr::INVALID_ARGUMENT;
  }

  return glcr::StaticCastRefPtr<MemoryObject>(
      glcr::MakeRefCounted<FixedMemoryObject>(physical_addr_ + offset, length));
}
