#include "memory/physical_memory.h"

#include "boot/boot_info.h"
#include "debug/debug.h"

namespace phys_mem {
namespace {

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
          dbgln("[PMM] Taking over from bootstrap, used: %x", bootstrap_used);
          size -= bootstrap_used;
        }
        AddMemoryRegion(base, size);
      }
    }
  }

  uint64_t AllocatePage() {
    if (front_ == nullptr) {
      panic("No available memory regions.");
    }

    if (front_->num_pages == 0) {
      panic("Bad state, empty memory block.");
    }

    uint64_t page = front_->base;
    front_->base += 0x1000;
    front_->num_pages--;
    if (front_->num_pages == 0) {
      MemBlock* temp = front_;
      front_ = front_->next;
      delete temp;
    }
    return page;
  }
  void FreePage(uint64_t page) { AddMemoryRegion(page, 0x1000); }

 private:
  void AddMemoryRegion(uint64_t base, uint64_t size) {
    MemBlock* block = new MemBlock{
        .next = front_,
        .base = base,
        .num_pages = size >> 12,
    };
    front_ = block;
  }
  struct MemBlock {
    MemBlock* next = nullptr;
    uint64_t base = 0;
    uint64_t num_pages = 0;
  };

  MemBlock* front_ = nullptr;
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
    if (entry.type == 0 && entry.length >= 0x3000) {
      gBootstrap.init_page = entry.base;
      gBootstrap.next_page = entry.base;
      gBootstrap.max_page = entry.base + 0x3000;
      gBootstrapEnabled = true;
      return;
    }
  }
}

void InitPhysicalMemoryManager() { gPmm = new PhysicalMemoryManager(); }

uint64_t AllocatePage() {
  if (gPmm != nullptr) {
    return gPmm->AllocatePage();
  }
  if (!gBootstrapEnabled) {
    panic("No Bootstrap Memory Manager");
  }

  dbgln("[PMM] Boostrap Alloc!");

  uint64_t page = gBootstrap.next_page;
  if (page == gBootstrap.max_page) {
    panic("Bootstrap Memory Manager OOM");
  }
  gBootstrap.next_page += 0x1000;

  return page;
}

}  // namespace phys_mem
