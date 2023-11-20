#include "mammoth/memory_region.h"

#include <zcall.h>

#include "mammoth/debug.h"
#include "mammoth/init.h"

MappedMemoryRegion MappedMemoryRegion::DirectPhysical(uint64_t paddr,
                                                      uint64_t size) {
  uint64_t vmmo_cap;
  check(ZMemoryObjectCreatePhysical(paddr, size, &vmmo_cap));

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));

  return MappedMemoryRegion(vmmo_cap, paddr, vaddr, size);
}

MappedMemoryRegion MappedMemoryRegion::ContiguousPhysical(uint64_t size) {
  uint64_t vmmo_cap, paddr;
  check(ZMemoryObjectCreateContiguous(size, &vmmo_cap, &paddr));

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));

  return MappedMemoryRegion(vmmo_cap, paddr, vaddr, size);
}

MappedMemoryRegion MappedMemoryRegion::Default(uint64_t size) {
  uint64_t vmmo_cap;
  check(ZMemoryObjectCreate(size, &vmmo_cap));

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));

  return MappedMemoryRegion(vmmo_cap, 0, vaddr, size);
}

MappedMemoryRegion MappedMemoryRegion::FromCapability(z_cap_t vmmo_cap) {
  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));

  // FIXME: get the size here.
  return MappedMemoryRegion(vmmo_cap, 0, vaddr, 0);
}

OwnedMemoryRegion::OwnedMemoryRegion(OwnedMemoryRegion&& other)
    : OwnedMemoryRegion(other.vmmo_cap_, other.paddr_, other.vaddr_,
                        other.size_) {
  other.vmmo_cap_ = 0;
  other.paddr_ = 0;
  other.vaddr_ = 0;
  other.size_ = 0;
}

OwnedMemoryRegion& OwnedMemoryRegion::operator=(OwnedMemoryRegion&& other) {
  if (vmmo_cap_) {
    check(ZCapRelease(vmmo_cap_));
  }
  vmmo_cap_ = other.vmmo_cap_;
  paddr_ = other.paddr_;
  vaddr_ = other.vaddr_;
  size_ = other.size_;
  other.vmmo_cap_ = 0;
  other.paddr_ = 0;
  other.vaddr_ = 0;
  other.size_ = 0;
  return *this;
}

OwnedMemoryRegion::~OwnedMemoryRegion() {
  if (vmmo_cap_ != 0) {
    check(ZCapRelease(vmmo_cap_));
  }
}

OwnedMemoryRegion OwnedMemoryRegion::FromCapability(z_cap_t vmmo_cap) {
  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));

  // FIXME: get the size here.
  return OwnedMemoryRegion(vmmo_cap, 0, vaddr, 0);
}
