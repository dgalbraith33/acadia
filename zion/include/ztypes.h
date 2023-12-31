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
const uint64_t kZionProcessWait = 0x3;

// Thread Calls.
const uint64_t kZionThreadCreate = 0x10;
const uint64_t kZionThreadStart = 0x11;
const uint64_t kZionThreadExit = 0x12;
const uint64_t kZionThreadWait = 0x13;
const uint64_t kZionThreadSleep = 0x14;

// Memory Calls
const uint64_t kZionAddressSpaceMap = 0x21;
const uint64_t kZionAddressSpaceUnmap = 0x22;

const uint64_t kZionMemoryObjectCreate = 0x30;
const uint64_t kZionMemoryObjectCreatePhysical = 0x31;
const uint64_t kZionMemoryObjectCreateContiguous = 0x32;

const uint64_t kZionMemoryObjectDuplicate = 0x38;
const uint64_t kZionMemoryObjectInspect = 0x39;

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

// Capability Calls
const uint64_t kZionCapDuplicate = 0x70;
const uint64_t kZionCapRelease = 0x71;

// Syncronization Calls
const uint64_t kZionMutexCreate = 0x80;
const uint64_t kZionMutexLock = 0x81;
const uint64_t kZionMutexRelease = 0x82;
const uint64_t kZionSemaphoreCreate = 0x83;
const uint64_t kZionSemaphoreWait = 0x84;
const uint64_t kZionSemaphoreSignal = 0x85;

// Debugging Calls.
const uint64_t kZionDebug = 0x1'0000;

// Irq Types
const uint64_t kZIrqKbd = 0x22;
const uint64_t kZIrqPci1 = 0x30;
const uint64_t kZIrqPci2 = 0x31;
const uint64_t kZIrqPci3 = 0x32;
const uint64_t kZIrqPci4 = 0x33;

/* ------------------------------
 * Capability Types
 * ------------------------------*/

typedef uint64_t z_cap_t;
typedef uint64_t z_perm_t;

const uint64_t kZionInvalidCapability = 0x0;

// General Capability Permissions
const uint64_t kZionPerm_Write = 0x1;
const uint64_t kZionPerm_Read = 0x2;

const uint64_t kZionPerm_Transmit = 0x10;
const uint64_t kZionPerm_Duplicate = 0x20;

// Capability Specific Permissions

// Permissions held on process capabilities.
const uint64_t kZionPerm_SpawnProcess = 0x100;
const uint64_t kZionPerm_SpawnThread = 0x200;

// Permissions on mutexes.
const uint64_t kZionPerm_Lock = 0x100;
const uint64_t kZionPerm_Release = 0x200;
// Permissions on semaphores.
const uint64_t kZionPerm_Wait = 0x100;
const uint64_t kZionPerm_Signal = 0x200;

const z_perm_t kZionPerm_None = 0;
const z_perm_t kZionPerm_All = -1;

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
#define Z_BOOT_VICTORIA_FALLS_VMMO 0x4200'0001
#define Z_BOOT_PCI_VMMO 0x4200'0002
#define Z_BOOT_FRAMEBUFFER_INFO_VMMO 0x4200'0003

struct ZFramebufferInfo {
  uint64_t address_phys;
  uint64_t width;
  uint64_t height;
  uint64_t pitch;
  uint16_t bpp;
  uint8_t memory_model;
  uint8_t red_mask_size;
  uint8_t red_mask_shift;
  uint8_t green_mask_size;
  uint8_t green_mask_shift;
  uint8_t blue_mask_size;
  uint8_t blue_mask_shift;
};
