#pragma once

#include <stdint.h>
#include <ztypes.h>

namespace mmth {
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
  // TODO: Consider making this its own class.
  static OwnedMemoryRegion ContiguousPhysical(uint64_t size, uint64_t* paddr);
  static OwnedMemoryRegion DirectPhysical(uint64_t paddr, uint64_t size);

  uint64_t vaddr() const { return vaddr_; }
  uint64_t size() const { return size_; }

  z_cap_t cap() const { return vmmo_cap_; }
  z_cap_t DuplicateCap();

  bool empty() const { return vmmo_cap_ == 0; }
  explicit operator bool() const { return vmmo_cap_ != 0; }

 private:
  OwnedMemoryRegion(uint64_t vmmo_cap, uint64_t vaddr, uint64_t size)
      : vmmo_cap_(vmmo_cap), vaddr_(vaddr), size_(size) {}
  uint64_t vmmo_cap_ = 0;
  uint64_t vaddr_ = 0;
  // TODO: We may want to differentiate between VMMO size and mapped size?
  uint64_t size_ = 0;
};

}  // namespace mmth
