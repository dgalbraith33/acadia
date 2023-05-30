#include "memory/paging_util.h"

#include "boot/boot_info.h"
#include "debug/debug.h"
#include "memory/physical_memory.h"

#define PRESENT_BIT 0x1
#define READ_WRITE_BIT 0x2
#define USER_MODE_BIT 0x4;
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
  return addr & 0xFF8;
}

uint64_t* Pml4Entry(uint64_t cr3, uint64_t addr) {
  cr3 += boot::GetHigherHalfDirectMap();
  return reinterpret_cast<uint64_t*>(cr3 |
                                     ShiftForEntryIndexing(addr, PML_OFFSET));
}

uint64_t* PageDirectoryPointerEntry(uint64_t pdp_phys, uint64_t addr) {
  pdp_phys += boot::GetHigherHalfDirectMap();
  pdp_phys &= ~0xFFF;
  return reinterpret_cast<uint64_t*>(pdp_phys |
                                     ShiftForEntryIndexing(addr, PDP_OFFSET));
}

uint64_t* PageDirectoryEntry(uint64_t pd_phys, uint64_t addr) {
  pd_phys += boot::GetHigherHalfDirectMap();
  pd_phys &= ~0xFFF;
  return reinterpret_cast<uint64_t*>(pd_phys |
                                     ShiftForEntryIndexing(addr, PD_OFFSET));
}

uint64_t* PageTableEntry(uint64_t pt_phys, uint64_t addr) {
  pt_phys += boot::GetHigherHalfDirectMap();
  pt_phys &= ~0xFFF;
  return reinterpret_cast<uint64_t*>(pt_phys |
                                     ShiftForEntryIndexing(addr, PT_OFFSET));
}

uint64_t PagePhysIfResident(uint64_t cr3, uint64_t virt) {
  uint64_t* pml4_entry = Pml4Entry(cr3, virt);
  if (!(*pml4_entry & PRESENT_BIT)) {
    return false;
  }
  uint64_t* pdp_entry = PageDirectoryPointerEntry(*pml4_entry, virt);
  if (!(*pdp_entry & PRESENT_BIT)) {
    return false;
  }
  uint64_t* pd_entry = PageDirectoryEntry(*pdp_entry, virt);
  if (!(*pd_entry & PRESENT_BIT)) {
    return false;
  }
  uint64_t* pt_entry = PageTableEntry(*pd_entry, virt);
  if (!(*pt_entry & PRESENT_BIT)) {
    return false;
  }
  return *pt_entry & ~0xFFF;
}

uint64_t MapPage(uint64_t cr3, uint64_t virt) {
  uint64_t access_bits = PRESENT_BIT | READ_WRITE_BIT;
  uint64_t higher_half = 0xffff8000'00000000;
  if ((virt & higher_half) != higher_half) {
    access_bits |= USER_MODE_BIT;
  }

  uint64_t* pml4_entry = Pml4Entry(cr3, virt);
  if (!(*pml4_entry & PRESENT_BIT)) {
    uint64_t page = phys_mem::AllocatePage();
    *pml4_entry = page | access_bits;
    ZeroOutPage(PageDirectoryPointerEntry(*pml4_entry, virt));
  }
  uint64_t* pdp_entry = PageDirectoryPointerEntry(*pml4_entry, virt);
  if (!(*pdp_entry & PRESENT_BIT)) {
    uint64_t page = phys_mem::AllocatePage();
    *pdp_entry = page | access_bits;
    ZeroOutPage(PageDirectoryEntry(*pdp_entry, virt));
  }
  uint64_t* pd_entry = PageDirectoryEntry(*pdp_entry, virt);
  if (!(*pd_entry & PRESENT_BIT)) {
    uint64_t page = phys_mem::AllocatePage();
    *(pd_entry) = page | access_bits;
    ZeroOutPage(PageTableEntry(*pd_entry, virt));
  }

  uint64_t* pt_entry = PageTableEntry(*pd_entry, virt);
  if (!(*pt_entry & PRESENT_BIT)) {
    uint64_t phys = phys_mem::AllocatePage();
    *pt_entry = PageAlign(phys) | access_bits;
    ZeroOutPage(reinterpret_cast<uint64_t*>(boot::GetHigherHalfDirectMap() +
                                            PageAlign(phys)));
    return phys;
  } else {
    panic("Page already allocated.");
    return 0;
  }
}

uint64_t Pml4Index(uint64_t addr) { return (addr >> PML_OFFSET) & 0x1FF; }

uint64_t CurrCr3() {
  uint64_t pml4_addr = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(pml4_addr));
  return pml4_addr;
}

}  // namespace

void InitializePml4(uint64_t pml4_physical_addr) {
  uint64_t* pml4_virtual = reinterpret_cast<uint64_t*>(
      boot::GetHigherHalfDirectMap() + pml4_physical_addr);
  uint64_t curr_cr3 = CurrCr3();

  // Map the kernel entry.
  // This should contain the heap at 0xFFFFFFFF'40000000
  uint64_t kernel_addr = 0xFFFFFFFF'80000000;
  pml4_virtual[Pml4Index(kernel_addr)] = *Pml4Entry(curr_cr3, kernel_addr);

  // Map the HHDM.
  // This is necessary to access values off of the kernel stack.
  uint64_t hhdm = boot::GetHigherHalfDirectMap();
  pml4_virtual[Pml4Index(hhdm)] = *Pml4Entry(curr_cr3, hhdm);
}

uint64_t AllocatePageIfNecessary(uint64_t addr, uint64_t cr3) {
  if (cr3 == 0) {
    cr3 = CurrCr3();
  }
  uint64_t phys = PagePhysIfResident(cr3, addr);
  if (phys) {
    return phys;
  }
  return MapPage(cr3, addr);
}

void EnsureResident(uint64_t addr, uint64_t size) {
  uint64_t max = addr + size;
  addr = PageAlign(addr);
  while (addr < max) {
    AllocatePageIfNecessary(addr);
    addr += 0x1000;
  }
}

void CopyIntoNonResidentProcess(uint64_t base, uint64_t size, uint64_t dest_cr3,
                                uint64_t dest_virt) {
  if (size > 0x1000) {
    panic("Unimplemented NR copy > 1 page");
  }
  if (dest_virt & 0xFFF) {
    panic("Unimplemented NR copy to non page aligned");
  }
  uint64_t phys = AllocatePageIfNecessary(dest_virt, dest_cr3);
  uint8_t* src = reinterpret_cast<uint8_t*>(base);
  uint8_t* dest =
      reinterpret_cast<uint8_t*>(phys + boot::GetHigherHalfDirectMap());

  for (uint64_t i = 0; i < size; i++) {
    dest[i] = src[i];
  }
}
