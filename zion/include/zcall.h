#pragma once

#include <stdint.h>

#define Z_THREAD_EXIT 0x01

#define Z_PROCESS_SPAWN 0x10

#define Z_DEBUG_PRINT 0x100

uint64_t ZDebug(const char* message);

// TODO: Move structs into an internal header.
struct ZProcessSpawnReq {
  uint64_t elf_base;
  uint64_t elf_size;
};

uint64_t ZProcessSpawn(uint64_t elf_base, uint64_t elf_size);
