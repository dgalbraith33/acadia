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

uint64_t AllocatePage() {
  if (!gBootstrapEnabled) {
    panic("No Bootstrap Memory Manager");
  }

  uint64_t page = gBootstrap.next_page;
  if (page == gBootstrap.max_page) {
    panic("Bootstrap Memory Manager OOM");
  }
  gBootstrap.next_page += 0x1000;

  return page;
}

}  // namespace phys_mem
