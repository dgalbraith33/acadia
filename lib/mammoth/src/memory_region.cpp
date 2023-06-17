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
