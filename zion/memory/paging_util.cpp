#include "memory/paging_util.h"

#include "boot/boot_info.h"

#define PRESENT_BIT 0x1
#define READ_WRITE_BIT 0x2

void InitPaging() {
  uint64_t pml4_addr = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(pml4_addr));
  InitializePml4(pml4_addr);
}

void InitializePml4(uint64_t pml4_physical_addr) {
  uint64_t* pml4_virtual = reinterpret_cast<uint64_t*>(
      boot::GetHigherHalfDirectMap() + pml4_physical_addr);

  uint64_t recursive_entry = pml4_physical_addr | PRESENT_BIT | READ_WRITE_BIT;
  pml4_virtual[0x1FE] = recursive_entry;
}
