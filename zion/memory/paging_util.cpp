#include "memory/paging_util.h"

#include "boot/boot_info.h"
#include "debug/debug.h"

#define PRESENT_BIT 0x1
#define READ_WRITE_BIT 0x2
#define SIGN_EXT 0xFFFF0000'00000000
#define RECURSIVE ((uint64_t)0x1FE)
#define PML_OFFSET 39
#define PDP_OFFSET 30
#define PD_OFFSET 21
#define PT_OFFSET 12

// How to recursively index into each page table structure assuming
// the PML4 is recursively mapped at the 510th entry (0x1FE).
#define PML_RECURSE 0xFFFFFF7F'BFDFE000
#define PDP_RECURSE 0xFFFFFF7F'BFC00000
#define PD_RECURSE 0xFFFFFF7F'80000000
#define PT_RECURSE 0xFFFFFF00'00000000

namespace {

uint64_t PageAlign(uint64_t addr) { return addr & ~0xFFF; }
uint64_t* PageAlign(uint64_t* addr) {
  return reinterpret_cast<uint64_t*>(reinterpret_cast<uint64_t>(addr) & ~0xFFF);
}

void ZeroOutPage(uint64_t* ptr) {
  ptr = PageAlign(ptr);
  for (uint64_t i = 0; i < 512; i++) {
    ptr[i] = 0;
  }
}

uint64_t ShiftForEntryIndexing(uint64_t addr, uint64_t offset) {
  addr &= ~0xFFFF0000'00000000;
  addr >>= offset;
  addr <<= 3;
  return addr;
}
}  // namespace

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

void AllocatePageDirectoryPointer(uint64_t addr);
void AllocatePageDirectory(uint64_t addr);
void AllocatePageTable(uint64_t addr);
void AllocatePage(uint64_t addr) { panic("Page Allocation Not Implemented."); }

void EnsureResident(uint64_t addr, uint64_t size) {
  uint64_t max = addr + size;
  addr = PageAlign(addr);
  while (addr < max) {
    if (!PageLoaded(addr)) {
      AllocatePage(addr);
    }
    addr += 0x1000;
  }
}

uint64_t* Pml4Entry(uint64_t addr) {
  return reinterpret_cast<uint64_t*>(PML_RECURSE |
                                     ShiftForEntryIndexing(addr, PML_OFFSET));
}

uint64_t* PageDirectoryPointerEntry(uint64_t addr) {
  return reinterpret_cast<uint64_t*>(PDP_RECURSE |
                                     ShiftForEntryIndexing(addr, PDP_OFFSET));
}

uint64_t* PageDirectoryEntry(uint64_t addr) {
  return reinterpret_cast<uint64_t*>(PD_RECURSE |
                                     ShiftForEntryIndexing(addr, PD_OFFSET));
}

uint64_t* PageTableEntry(uint64_t addr) {
  return reinterpret_cast<uint64_t*>(PT_RECURSE |
                                     ShiftForEntryIndexing(addr, PT_OFFSET));
}

bool PageDirectoryPointerLoaded(uint64_t addr) {
  return *Pml4Entry(addr) & PRESENT_BIT;
}

bool PageDirectoryLoaded(uint64_t addr) {
  return PageDirectoryPointerLoaded(addr) &&
         (*PageDirectoryPointerEntry(addr) & PRESENT_BIT);
}

bool PageTableLoaded(uint64_t addr) {
  return PageDirectoryLoaded(addr) && (*PageDirectoryEntry(addr) & PRESENT_BIT);
}

bool PageLoaded(uint64_t addr) {
  return PageTableLoaded(addr) && (*PageTableEntry(addr) & PRESENT_BIT);
}
