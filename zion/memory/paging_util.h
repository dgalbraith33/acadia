#pragma once

#include <stdint.h>

void InitPaging();
void InitializePml4(uint64_t pml4_physical_addr);

void AllocatePageDirectoryPointer(uint64_t addr);
void AllocatePageDirectory(uint64_t addr);
void AllocatePageTable(uint64_t addr);
void AllocatePage(uint64_t addr);
void EnsureResident(uint64_t addr, uint64_t size);

uint64_t* Pml4Entry(uint64_t addr);
uint64_t* PageDirectoryPointerEntry(uint64_t addr);
uint64_t* PageDirectoryEntry(uint64_t addr);
uint64_t* PageTableEntry(uint64_t addr);

bool PageDirectoryPointerLoaded(uint64_t addr);
bool PageDirectoryLoaded(uint64_t addr);
bool PageTableLoaded(uint64_t addr);
bool PageLoaded(uint64_t addr);
