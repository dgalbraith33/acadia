#include "util/memory_region.h"

#include <zcall.h>

#include "util/debug.h"
#include "util/init.h"

namespace mmth {

OwnedMemoryRegion::OwnedMemoryRegion(OwnedMemoryRegion&& other)
    : OwnedMemoryRegion(other.vmmo_cap_, other.vaddr_, other.size_) {
  other.vmmo_cap_ = 0;
  other.vaddr_ = 0;
  other.size_ = 0;
}

OwnedMemoryRegion& OwnedMemoryRegion::operator=(OwnedMemoryRegion&& other) {
  if (vmmo_cap_) {
    check(ZCapRelease(vmmo_cap_));
  }
  vmmo_cap_ = other.vmmo_cap_;
  vaddr_ = other.vaddr_;
  size_ = other.size_;
  other.vmmo_cap_ = 0;
  other.vaddr_ = 0;
  other.size_ = 0;
  return *this;
}

OwnedMemoryRegion::~OwnedMemoryRegion() {
  if (vmmo_cap_ != 0) {
    check(ZAddressSpaceUnmap(gSelfVmasCap, vaddr_, vaddr_ + size_));
    check(ZCapRelease(vmmo_cap_));
  }
}

OwnedMemoryRegion OwnedMemoryRegion::FromCapability(z_cap_t vmmo_cap) {
  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, 0, &vaddr));

  uint64_t size;
  check(ZMemoryObjectInspect(vmmo_cap, &size));
  // FIXME: get the size here.
  return OwnedMemoryRegion(vmmo_cap, vaddr, size);
}

OwnedMemoryRegion OwnedMemoryRegion::ContiguousPhysical(uint64_t size,
                                                        uint64_t* paddr) {
  uint64_t vmmo_cap;
  check(ZMemoryObjectCreateContiguous(size, &vmmo_cap, paddr));

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, 0, &vaddr));

  return OwnedMemoryRegion(vmmo_cap, vaddr, size);
}

OwnedMemoryRegion OwnedMemoryRegion::DirectPhysical(uint64_t paddr,
                                                    uint64_t size) {
  uint64_t vmmo_cap;
  check(ZMemoryObjectCreatePhysical(paddr, size, &vmmo_cap));

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, 0, &vaddr));
  return OwnedMemoryRegion(vmmo_cap, vaddr, size);
}

z_cap_t OwnedMemoryRegion::DuplicateCap() {
  z_cap_t cap;
  check(ZCapDuplicate(vmmo_cap_, kZionPerm_All, &cap));
  return cap;
}

OwnedMemoryRegion OwnedMemoryRegion::Duplicate() {
  return OwnedMemoryRegion::FromCapability(DuplicateCap());
}

}  // namespace mmth
