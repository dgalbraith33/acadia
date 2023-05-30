#pragma once

#include <stdint.h>

// Loads the elf program and returns its entry point.
uint64_t LoadElfProgram(uint64_t cr3, uint64_t base, uint64_t length);
