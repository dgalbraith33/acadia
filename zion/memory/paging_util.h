#pragma once

#include <stdint.h>

void InitializePml4(uint64_t pml4_physical_addr);

void AllocatePageIfNecessary(uint64_t addr, uint64_t cr3 = 0);
void EnsureResident(uint64_t addr, uint64_t size);
