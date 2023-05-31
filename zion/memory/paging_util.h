#pragma once

#include <stdint.h>

#include "scheduler/process.h"

void InitializePml4(uint64_t pml4_physical_addr);

uint64_t AllocatePageIfNecessary(uint64_t addr, uint64_t cr3 = 0);
void EnsureResident(uint64_t addr, uint64_t size);

void CopyIntoNonResidentProcess(uint64_t base, uint64_t size,
                                Process& dest_proc, uint64_t dest_virt);
