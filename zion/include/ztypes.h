#pragma once

#include <stdint.h>

/* ------------------------------
 * Error Types
 *
 * Bit 31 always set to 1 to
 * distinguish from user-space errors.
 * ------------------------------*/
#define Z_OK 0x0

#define Z_ERR_NOT_FOUND 0x1000'0001
#define Z_ERR_INVALID 0x1000'0002
#define Z_ERR_DENIED 0x1000'0003
#define Z_ERR_UNIMPLEMENTED 0x1000'0004
#define Z_ERR_BUFF_SIZE 001000'0005
#define Z_ERR_NULL 0x1000'0006
#define Z_ERR_EXISTS 0x1000'0007
#define Z_ERR_EMPTY 0x1000'0008

#define Z_ERR_CAP_NOT_FOUND 0x1001'0000
#define Z_ERR_CAP_TYPE 0x1001'0001
#define Z_ERR_CAP_DENIED 0x1001'0002

typedef uint64_t z_err_t;

/* ------------------------------
 * Syscall Types
 * ------------------------------*/

// Process Calls.
#define Z_PROCESS_EXIT 0x01
#define Z_PROCESS_SPAWN 0x02
#define Z_PROCESS_START 0x03

// Thread Calls.
#define Z_THREAD_CREATE 0x10
#define Z_THREAD_START 0x11
#define Z_THREAD_EXIT 0x12

// Memory Calls
#define Z_ADDRESS_SPACE_MAP 0x21
#define Z_ADDRESS_SPACE_UNMAP 0x22

#define Z_MEMORY_OBJECT_CREATE 0x30
#define Z_MEMORY_OBJECT_CREATE_PHYSICAL 0x31

#define Z_TEMP_PCIE_CONFIG_OBJECT_CREATE 0x3F

// IPC Calls
#define Z_CHANNEL_CREATE 0x40
#define Z_CHANNEL_SEND 0x41
#define Z_CHANNEL_RECV 0x42
#define Z_CHANNEL_SENDRECV 0x43

#define Z_PORT_CREATE 0x50
#define Z_PORT_SEND 0x51
#define Z_PORT_RECV 0x52
#define Z_PORT_POLL 0x53

#define Z_IRQ_REGISTER 0x58

#define Z_IRQ_PCI_BASE 0x30

// Capability Calls
#define Z_CAP_DUPLICATE 0x60

// Debugging Calls.
#define Z_DEBUG_PRINT 0x10000000

/* ------------------------------
 * Capability Types
 * ------------------------------*/

typedef uint64_t z_cap_t;

#define Z_INVALID 0x0

// General Capability Permissions
#define ZC_WRITE 0x01
#define ZC_READ 0x02

// Capability Specific Permissions
#define ZC_PROC_SPAWN_PROC 0x100
#define ZC_PROC_SPAWN_THREAD 0x200

/* ------------------------------
 * Process Init Types
 *
 * Used to pull capabilites off
 * the initialization port.
 *
 * Start at a high number only to
 * make them distinctive as a raw
 * value.
 * ------------------------------*/
#define Z_INIT_SELF_PROC 0x4000'0000
#define Z_INIT_SELF_VMAS 0x4000'0001

#define Z_INIT_CHANNEL 0x4100'0000

#define Z_BOOT_DENALI_VMMO 0x4200'0000
