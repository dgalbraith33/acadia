#pragma once

#include <stdint.h>

namespace phys_mem {

// Called before creating a kernel heap object
// that allows dynamic allocation. The real
// PhysicalMemoryManager requires some allocations
// to initialize so we need this first.
void InitBootstrapPageAllocation();

// Initializes the main physical memory manager.
void InitPhysicalMemoryManager();

uint64_t AllocatePage();
uint64_t AllocateAndZeroPage();

uint64_t AllocateContinuous(uint64_t num_pages);
void FreePage(uint64_t page);

}  // namespace phys_mem
