#pragma once

#include <stdint.h>

#define Z_INVALID 0x0

#define ZC_WRITE 0x01
#define ZC_READ 0x02

#define Z_PROCESS_EXIT 0x01
#define Z_PROCESS_SPAWN 0x02
#define Z_PROCESS_START 0x03

#define Z_PROCESS_SPAWN_ELF 0x1'00000002

#define ZC_PROC_SPAWN_PROC 0x100
#define ZC_PROC_SPAWN_THREAD 0x101

#define Z_THREAD_CREATE 0x10
#define Z_THREAD_START 0x11
#define Z_THREAD_EXIT 0x12

#define Z_DEBUG_PRINT 0x10000000

uint64_t ZDebug(const char* message);

// TODO: Move structs into an internal header.
struct ZProcessSpawnElfReq {
  uint64_t cap_id;
  uint64_t elf_base;
  uint64_t elf_size;
};

uint64_t ZProcessSpawnElf(uint64_t cap_id, uint64_t elf_base,
                          uint64_t elf_size);
