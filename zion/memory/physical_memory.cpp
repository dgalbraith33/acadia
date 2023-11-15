#include "memory/physical_memory.h"

#include <glacier/container/linked_list.h>

#include "boot/boot_info.h"
#include "debug/debug.h"

#define K_PHYS_DEBUG 0

namespace phys_mem {
namespace {

uint64_t* PageAlign(uint64_t* addr) {
  return reinterpret_cast<uint64_t*>(reinterpret_cast<uint64_t>(addr) & ~0xFFF);
}

void ZeroOutPage(uint64_t* ptr) {
  ptr = PageAlign(ptr);
  for (uint64_t i = 0; i < 512; i++) {
    ptr[i] = 0;
  }
}

struct BootstrapMemory {
  uint64_t init_page = 0;
  uint64_t next_page = 0;
  uint64_t max_page = 0;
};

static BootstrapMemory gBootstrap;
static bool gBootstrapEnabled = false;

class PhysicalMemoryManager {
 public:
  // Reads the memory map and takes
  // control of the available regions.
  PhysicalMemoryManager() {
    const limine_memmap_response& memmap = boot::GetMemoryMap();
    for (uint64_t i = 0; i < memmap.entry_count; i++) {
      const limine_memmap_entry& entry = *memmap.entries[i];
      if (entry.type == 0) {
        uint64_t base = entry.base;
        uint64_t size = entry.length;
        if (base == gBootstrap.init_page) {
          base = gBootstrap.next_page;
          uint64_t bootstrap_used = gBootstrap.next_page - gBootstrap.init_page;
#if K_PHYS_DEBUG
          dbgln("[PMM] Taking over from bootstrap, used: {x}", bootstrap_used);
#endif
          size -= bootstrap_used;
        }
        AddMemoryRegion(base, size);
      }
    }
  }

  uint64_t AllocatePage() {
    if (memory_blocks.size() == 0) {
      panic("No available memory regions.");
    }

    while (memory_blocks.PeekFront().num_pages == 0) {
      memory_blocks.PopFront();
    }

    MemBlock& block = memory_blocks.PeekFront();
    uint64_t page = block.base;
    block.base += 0x1000;
    block.num_pages--;
    if (block.num_pages == 0) {
      memory_blocks.PopFront();
    }
#if K_PHYS_DEBUG
    dbgln("Single {x}", page);
#endif
    allocated_pages_ += 1;
    return page;
  }
  uint64_t AllocateContinuous(uint64_t num_pages) {
    if (memory_blocks.size() == 0) {
      panic("No available memory regions.");
    }

    MemBlock& block = memory_blocks.PeekFront();
    if (block.num_pages == 0) {
      panic("Bad state, empty memory block.");
    }

    auto iter = memory_blocks.begin();
    while (iter != memory_blocks.end() && iter->num_pages < num_pages) {
      dbgln("Skipping block of size {} seeking {}", iter->num_pages, num_pages);
      iter = iter.next();
    }

    if (iter == memory_blocks.end()) {
      panic("No memory regions to allocate");
    }

    uint64_t page = iter->base;
    iter->base += num_pages * 0x1000;
    iter->num_pages -= num_pages;
#if K_PHYS_DEBUG
    dbgln("Continuous {x}:{}", page, num_pages);
#endif
    allocated_pages_ += num_pages;
    return page;
  }
  void FreePage(uint64_t page) {
    AddMemoryRegion(page, 0x1000);
    allocated_pages_--;
  }

  uint64_t AllocatedPages() { return allocated_pages_; }

  uint64_t AvailablePages() {
    uint64_t available = 0;
    for (auto iter = memory_blocks.begin(); iter != memory_blocks.end();
         iter = iter.next()) {
      available += iter->num_pages;
    }
    return available;
  }

 private:
  struct MemBlock {
    uint64_t base = 0;
    uint64_t num_pages = 0;
  };

  glcr::LinkedList<MemBlock> memory_blocks;

  uint64_t allocated_pages_ = 0;

  void AddMemoryRegion(uint64_t base, uint64_t size) {
    MemBlock block{
        .base = base,
        .num_pages = size >> 12,
    };
    memory_blocks.PushFront(block);
  }
};

static PhysicalMemoryManager* gPmm = nullptr;

};  // namespace

void InitBootstrapPageAllocation() {
  const limine_memmap_response& memmap = boot::GetMemoryMap();
  for (uint64_t i = 0; i < memmap.entry_count; i++) {
    const limine_memmap_entry& entry = *memmap.entries[i];
    // We may want to chose a high address space to not limit
    // the number of buffers we can allocate later but
    // if we limit the number of pages this should be fine.
    // Currently set to the minimum of 3 for one kernel heap allocation:
    // PageDirectory + PageTable + Page
    if (entry.type == 0 && entry.length >= 0x5000) {
      gBootstrap.init_page = entry.base;
      gBootstrap.next_page = entry.base;
      gBootstrap.max_page = entry.base + 0x4000;
      gBootstrapEnabled = true;
      return;
    }
  }
}

void InitPhysicalMemoryManager() { gPmm = new PhysicalMemoryManager(); }

glcr::StringView MemoryRegionStr(uint64_t type) {
  switch (type) {
    case LIMINE_MEMMAP_USABLE:
      return "USABLE";
    case LIMINE_MEMMAP_RESERVED:
      return "RESRVD";
    case LIMINE_MEMMAP_ACPI_RECLAIMABLE:
      return "ACPIREC";
    case LIMINE_MEMMAP_ACPI_NVS:
      return "ACPINVS";
    case LIMINE_MEMMAP_BAD_MEMORY:
      return "BADMEM";
    case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
      return "LIMINE";
    case LIMINE_MEMMAP_KERNEL_AND_MODULES:
      return "KERNEL";
    case LIMINE_MEMMAP_FRAMEBUFFER:
      return "FRMBFR";
    default:
      return "UNKNWN";
  }
}

void DumpRegions() {
  const limine_memmap_response& memmap = boot::GetMemoryMap();
  for (uint64_t i = 0; i < memmap.entry_count; i++) {
    const limine_memmap_entry& entry = *memmap.entries[i];
    dbgln("Region({}) at {x}:{x} ({x})", MemoryRegionStr(entry.type),
          entry.base, entry.base + entry.length, entry.length);
  }
}

uint64_t AllocatePage() {
  if (gPmm != nullptr) {
    return gPmm->AllocatePage();
  }
  if (!gBootstrapEnabled) {
    panic("No Bootstrap Memory Manager");
  }

#if K_PHYS_DEBUG
  early_dbgln("[PMM] Boostrap Alloc!");
#endif

  uint64_t page = gBootstrap.next_page;
  if (page == gBootstrap.max_page) {
    panic("Bootstrap Memory Manager OOM");
  }
  gBootstrap.next_page += 0x1000;

  return page;
}

uint64_t AllocateAndZeroPage() {
  uint64_t paddr = AllocatePage();
  ZeroOutPage(
      reinterpret_cast<uint64_t*>(boot::GetHigherHalfDirectMap() + paddr));
  return paddr;
}

uint64_t AllocateContinuous(uint64_t num_continuous) {
  if (gPmm == nullptr) {
    panic("No physical memory manager!");
  }

  return gPmm->AllocateContinuous(num_continuous);
}

void DumpPhysicalMemoryUsage() {
  dbgln("Pages used: {} MiB, avail: {} MiB", gPmm->AllocatedPages() / 256,
        gPmm->AvailablePages() / 256);
}

}  // namespace phys_mem
