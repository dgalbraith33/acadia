#pragma once

#include <stdint.h>

#include "object/process.h"

void InitializePml4(uint64_t pml4_physical_addr);
void CleanupPml4(uint64_t pml4_physical_addr);

void MapPage(uint64_t cr3, uint64_t vaddr, uint64_t paddr);
void UnmapPage(uint64_t cr3, uint64_t vaddr);

uint64_t AllocatePageIfNecessary(uint64_t addr);
void EnsureResident(uint64_t addr, uint64_t size);
