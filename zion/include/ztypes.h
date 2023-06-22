#pragma once

#include <stdint.h>

// Error codes defined in glacier/status/error.h
typedef uint64_t z_err_t;

/* ------------------------------
 * Syscall Types
 * ------------------------------*/

// Process Calls.
const uint64_t kZionProcessExit = 0x1;
const uint64_t kZionProcessSpawn = 0x2;

// Thread Calls.
const uint64_t kZionThreadCreate = 0x10;
const uint64_t kZionThreadStart = 0x11;
const uint64_t kZionThreadExit = 0x12;

// Memory Calls
const uint64_t kZionAddressSpaceMap = 0x21;
const uint64_t kZionAddressSpaceUnMap = 0x21;

const uint64_t kZionMemoryObjectCreate = 0x30;
const uint64_t kZionMemoryObjectCreatePhysical = 0x31;
const uint64_t kZionMemoryObjectCreateContiguous = 0x32;

const uint64_t kZionTempPcieConfigObjectCreate = 0x3F;

// IPC Calls
const uint64_t kZionChannelCreate = 0x40;
const uint64_t kZionChannelSend = 0x41;
const uint64_t kZionChannelRecv = 0x42;
const uint64_t kZionChannelSendRecv = 0x43;

const uint64_t kZionPortCreate = 0x50;
const uint64_t kZionPortSend = 0x51;
const uint64_t kZionPortRecv = 0x52;
const uint64_t kZionPortPoll = 0x53;

const uint64_t kZionIrqRegister = 0x58;

const uint64_t kZionEndpointCreate = 0x60;
const uint64_t kZionEndpointSend = 0x61;
const uint64_t kZionEndpointRecv = 0x62;
const uint64_t kZionReplyPortSend = 0x63;
const uint64_t kZionReplyPortRecv = 0x64;
const uint64_t kZionEndpointCall = 0x65;

#define Z_IRQ_PCI_BASE 0x30

// Capability Calls
const uint64_t kZionCapDuplicate = 0x70;

// Debugging Calls.
const uint64_t kZionDebug = 0x1'0000;

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

#define Z_INIT_ENDPOINT 0x4100'0000

#define Z_BOOT_DENALI_VMMO 0x4200'0000
