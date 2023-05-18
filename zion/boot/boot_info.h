#pragma once

#include "boot/limine.h"

namespace boot {

const limine_memmap_response& GetMemoryMap();
uint64_t GetHigherHalfDirectMap();

}  // namespace boot
