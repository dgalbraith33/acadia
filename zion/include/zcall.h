#pragma once

#include <stdint.h>

#include "zerrors.h"

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

// Thread Calls.
#define Z_THREAD_CREATE 0x10
#define Z_THREAD_START 0x11
#define Z_THREAD_EXIT 0x12

// Memory Calls
#define Z_ADDRESS_SPACE_MAP 0x21
#define Z_ADDRESS_SPACE_UNMAP 0x22

#define Z_INIT_VMAS_SELF 0x20

#define Z_MEMORY_OBJECT_CREATE 0x30

#define Z_INIT_BOOT_VMMO 0x31

// IPC Calls
#define Z_CHANNEL_CREATE 0x40
#define Z_CHANNEL_SEND 0x41
#define Z_CHANNEL_RECV 0x42
#define Z_CHANNEL_SENDRECV 0x43

// Debugging Calls.
#define Z_DEBUG_PRINT 0x10000000

void ZProcessExit(uint64_t code);

[[nodiscard]] z_err_t ZProcessSpawn(uint64_t proc_cap, uint64_t bootstrap_cap,
                                    uint64_t* new_proc_cap,
                                    uint64_t* new_vmas_cap,
                                    uint64_t* new_bootstrap_cap);

// UNUSED for now, I think we can get away with just starting a thread.
[[nodiscard]] z_err_t ZProcessStart(uint64_t proc_cap, uint64_t thread_cap,
                                    uint64_t entry, uint64_t arg1,
                                    uint64_t arg2);

[[nodiscard]] z_err_t ZThreadCreate(uint64_t proc_cap, uint64_t* thread_cap);

[[nodiscard]] z_err_t ZThreadStart(uint64_t thread_cap, uint64_t entry,
                                   uint64_t arg1, uint64_t arg2);

void ZThreadExit();

[[nodiscard]] z_err_t ZAddressSpaceMap(uint64_t vmas_cap, uint64_t vmas_offset,
                                       uint64_t vmmo_cap, uint64_t* vaddr);
[[nodiscard]] z_err_t ZMemoryObjectCreate(uint64_t size, uint64_t* vmmo_cap);

[[nodiscard]] z_err_t ZChannelCreate(uint64_t* channel1, uint64_t* channel2);
[[nodiscard]] z_err_t ZChannelSend(uint64_t chan_cap, uint64_t type,
                                   uint64_t num_bytes, const uint8_t* bytes,
                                   uint64_t num_caps, const uint64_t* caps);
[[nodiscard]] z_err_t ZChannelRecv(uint64_t chan_cap, uint64_t num_bytes,
                                   uint8_t* bytes, uint64_t num_caps,
                                   uint64_t* caps, uint64_t* type,
                                   uint64_t* actual_bytes,
                                   uint64_t* actual_caps);

[[nodiscard]] z_err_t ZDebug(const char* message);
