#pragma once

#include <stdint.h>

#define Z_INVALID 0x0

#define ZC_WRITE 0x01
#define ZC_READ 0x02

// Process Calls.
#define Z_PROCESS_EXIT 0x01
#define Z_PROCESS_SPAWN 0x02
#define Z_PROCESS_START 0x03

#define Z_PROCESS_SPAWN_ELF 0x1'00000002

#define ZC_PROC_SPAWN_PROC 0x100
#define ZC_PROC_SPAWN_THREAD 0x101

#define Z_INIT_PROC_SELF 0x1

// Thread Calls.
#define Z_THREAD_CREATE 0x10
#define Z_THREAD_START 0x11
#define Z_THREAD_EXIT 0x12

// Debugging Calls.
#define Z_DEBUG_PRINT 0x10000000

uint64_t ZDebug(const char* message);

// TODO: Move structs into an internal header.
struct ZProcessSpawnElfReq {
  uint64_t cap_id;
  uint64_t elf_base;
  uint64_t elf_size;
};

// Creates a child process of the current one and
// starts its root thread from the provided ELF file.
uint64_t ZProcessSpawnElf(uint64_t cap_id, uint64_t elf_base,
                          uint64_t elf_size);

struct ZThreadCreateReq {
  uint64_t proc_cap;
};

struct ZThreadCreateResp {
  uint64_t thread_cap;
};

uint64_t ZThreadCreate(uint64_t proc_cap, uint64_t* thread_cap);

struct ZThreadStartReq {
  uint64_t thread_cap;
  uint64_t entry;
  uint64_t arg1;
  uint64_t arg2;
};

uint64_t ZThreadStart(uint64_t thread_cap, uint64_t entry, uint64_t arg1,
                      uint64_t arg2);

void ZThreadExit();
