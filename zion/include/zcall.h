#pragma once

#include <stdint.h>

#define Z_INVALID 0x0

#define ZC_WRITE 0x01
#define ZC_READ 0x02

// Process Calls.
#define Z_PROCESS_EXIT 0x01
#define Z_PROCESS_SPAWN 0x02
#define Z_PROCESS_START 0x03

#define ZC_PROC_SPAWN_PROC 0x100
#define ZC_PROC_SPAWN_THREAD 0x101

#define Z_INIT_PROC_SELF 0x1
#define Z_INIT_AS_SELF 0x2

// Thread Calls.
#define Z_THREAD_CREATE 0x10
#define Z_THREAD_START 0x11
#define Z_THREAD_EXIT 0x12

// Memory Calls
#define Z_ADDRESS_SPACE_MAP 0x21
#define Z_ADDRESS_SPACE_UNMAP 0x22

#define Z_MEMORY_OBJECT_CREATE 0x30

// Debugging Calls.
#define Z_DEBUG_PRINT 0x10000000

void ZProcessExit(uint64_t code);

[[nodiscard]] uint64_t ZProcessSpawn(uint64_t proc_cap, uint64_t* new_proc_cap,
                                     uint64_t* new_as_cap);

// UNUSED for now, I think we can get away with just starting a thread.
[[nodiscard]] uint64_t ZProcessStart(uint64_t proc_cap, uint64_t thread_cap,
                                     uint64_t entry, uint64_t arg1,
                                     uint64_t arg2);

[[nodiscard]] uint64_t ZThreadCreate(uint64_t proc_cap, uint64_t* thread_cap);

[[nodiscard]] uint64_t ZThreadStart(uint64_t thread_cap, uint64_t entry,
                                    uint64_t arg1, uint64_t arg2);

void ZThreadExit();

[[nodiscard]] uint64_t ZAddressSpaceMap(uint64_t as_cap, uint64_t offset,
                                        uint64_t mem_cap, uint64_t* vaddr);
[[nodiscard]] uint64_t ZMemoryObjectCreate(uint64_t size, uint64_t* mem_cap);

[[nodiscard]] uint64_t ZDebug(const char* message);
