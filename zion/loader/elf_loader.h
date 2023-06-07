#pragma once

#include <stdint.h>

#include "object/process.h"

// Loads the elf program and returns its entry point.
uint64_t LoadElfProgram(Process& dest_proc, uint64_t base, uint64_t length);
