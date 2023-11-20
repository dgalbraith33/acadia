#pragma once

#include <stdint.h>
#include <ztypes.h>

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
  uint64_t size() { return size_; }

  uint64_t cap() { return vmmo_cap_; }

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

/*
 * Memory Region class that unmaps its memory and releases its
 * capability when it goes out of scope.
 */
class OwnedMemoryRegion {
 public:
  OwnedMemoryRegion() = default;

  OwnedMemoryRegion(const OwnedMemoryRegion&) = delete;
  OwnedMemoryRegion& operator=(const OwnedMemoryRegion&) = delete;

  OwnedMemoryRegion(OwnedMemoryRegion&&);
  OwnedMemoryRegion& operator=(OwnedMemoryRegion&&);

  ~OwnedMemoryRegion();

  static OwnedMemoryRegion FromCapability(z_cap_t vmmo_cap);

  uint64_t vaddr() { return vaddr_; }
  uint64_t size() { return size_; }

  z_cap_t cap() { return vmmo_cap_; }
  z_cap_t DuplicateCap();

  bool empty() { return vmmo_cap_ != 0; }
  explicit operator bool() { return vmmo_cap_ != 0; }

 private:
  OwnedMemoryRegion(uint64_t vmmo_cap, uint64_t vaddr, uint64_t size)
      : vmmo_cap_(vmmo_cap), vaddr_(vaddr), size_(size) {}
  uint64_t vmmo_cap_ = 0;
  uint64_t vaddr_ = 0;
  uint64_t size_ = 0;
};
