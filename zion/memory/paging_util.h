#pragma once

#include <stdint.h>

void InitPaging();
void InitializePml4(uint64_t pml4_physical_addr);
