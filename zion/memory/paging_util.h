#pragma once

#include <stdint.h>

void InitPaging();
void InitializePml4(uint64_t pml4_physical_addr);

void AllocatePage(uint64_t addr);
void EnsureResident(uint64_t addr, uint64_t size);

bool PageLoaded(uint64_t addr);
