#pragma once

#include <stdint.h>

class MappedMemoryRegion {
 public:
  // FIXME: Introduce optional type to contain error or.
  static MappedMemoryRegion DirectPhysical(uint64_t phys_addr, uint64_t size);
  static MappedMemoryRegion ContiguousPhysical(uint64_t size);
  static MappedMemoryRegion Default(uint64_t size);

  MappedMemoryRegion() {}
  // TODO: Disallow copy before doing any cleanup here.
  ~MappedMemoryRegion() {}

  uint64_t paddr() { return paddr_; }
  uint64_t vaddr() { return vaddr_; }

  operator bool() { return vmmo_cap_ != 0; }

 private:
  MappedMemoryRegion(uint64_t vmmo_cap, uint64_t paddr, uint64_t vaddr,
                     uint64_t size)
      : vmmo_cap_(vmmo_cap), paddr_(paddr), vaddr_(vaddr), size_(size) {}
  uint64_t vmmo_cap_ = 0;
  uint64_t paddr_ = 0;
  uint64_t vaddr_ = 0;
  uint64_t size_ = 0;
};
