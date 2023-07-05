#pragma once

#include <stdint.h>

struct PciDeviceHeader {
  uint16_t vendor_id;
  uint16_t device_id;
  uint16_t command_reg;
  uint16_t status_reg;
  uint8_t revision;
  uint8_t prog_interface;
  uint8_t subclass;
  uint8_t class_code;
  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t bist;
  uint32_t bars[5];
  uint32_t abar;
  uint32_t reserved0;
  uint32_t subsystem_id;
  uint32_t expansion_rom;
  uint8_t cap_ptr;
  uint8_t reserved1[7];
  uint8_t interrupt_line;
  uint8_t interrupt_pin;
  uint8_t min_grant;
  uint8_t max_latency;
} __attribute__((packed));

struct PciMsiCap {
  uint8_t cap_id;
  uint8_t next_offset;
  uint8_t message_control;
  uint8_t reserved;
  uint64_t message_address;
  uint16_t message_data;
} __attribute__((packed));

struct AhciHba {
  uint32_t capabilities;
  uint32_t global_host_control;
  uint32_t interrupt_status;
  uint32_t port_implemented;
  uint32_t version;
  uint32_t ccc_ctl;  // 0x14, Command completion coalescing control
  uint32_t ccc_pts;  // 0x18, Command completion coalescing ports
  uint32_t em_loc;   // 0x1C, Enclosure management location
  uint32_t em_ctl;   // 0x20, Enclosure management control
  uint32_t capabilities_ext;
  uint32_t bohc;  // 0x28, BIOS/OS handoff control and status
} __attribute__((packed));

struct AhciPort {
  uint64_t command_list_base;
  uint64_t fis_base;
  uint32_t interrupt_status;
  uint32_t interrupt_enable;
  uint32_t command;
  uint32_t reserved;
  uint32_t task_file_data;
  uint32_t signature;
  uint32_t sata_status;
  uint32_t sata_control;
  uint32_t sata_error;
  uint32_t sata_active;
  uint32_t command_issue;
  uint32_t sata_notification;
  uint32_t fis_based_switching_ctl;
  uint32_t device_sleep;
} __attribute__((packed));

struct CommandHeader {
  uint16_t command;
  uint16_t prd_table_length;
  uint32_t prd_byte_count;
  uint64_t command_table_base_addr;
  uint64_t reserved1;
  uint64_t reserved2;
} __attribute__((packed));

struct CommandList {
  CommandHeader command_headers[32];
} __attribute__((packed));

struct PhysicalRegionDescriptor {
  uint64_t region_address;
  uint32_t reserved;
  // bit 0 must be one.
  // 21:0 is byte count
  // 31 is Interrupt on Completion
  uint32_t byte_count;
} __attribute__((packed));

struct CommandTable {
  uint8_t command_fis[64];
  uint8_t atapi_command[16];
  uint8_t reserved[48];
  PhysicalRegionDescriptor prdt[8];
} __attribute__((packed));

typedef enum {
  FIS_TYPE_REG_H2D = 0x27,    // Register FIS - host to device
  FIS_TYPE_REG_D2H = 0x34,    // Register FIS - device to host
  FIS_TYPE_DMA_ACT = 0x39,    // DMA activate FIS - device to host
  FIS_TYPE_DMA_SETUP = 0x41,  // DMA setup FIS - bidirectional
  FIS_TYPE_DATA = 0x46,       // Data FIS - bidirectional
  FIS_TYPE_BIST = 0x58,       // BIST activate FIS - bidirectional
  FIS_TYPE_PIO_SETUP = 0x5F,  // PIO setup FIS - device to host
  FIS_TYPE_DEV_BITS = 0xA1,   // Set device bits FIS - device to host
} FIS_TYPE;

struct DmaFis {
  // DWORD 0
  uint8_t fis_type;  // FIS_TYPE_DMA_SETUP

  uint8_t pmport : 4;  // Port multiplier
  uint8_t rsv0 : 1;    // Reserved
  uint8_t d : 1;       // Data transfer direction, 1 - device to host
  uint8_t i : 1;       // Interrupt bit
  uint8_t a : 1;       // Auto-activate. Specifies if DMA Activate FIS is needed

  uint8_t rsved[2];  // Reserved

  // DWORD 1&2

  uint64_t DMAbufferID;  // DMA Buffer Identifier. Used to Identify DMA buffer
                         // in host memory. SATA Spec says host specific and not
                         // in Spec. Trying AHCI spec might work.

  // DWORD 3
  uint32_t rsvd;  // More reserved

  // DWORD 4
  uint32_t DMAbufOffset;  // Byte offset into buffer. First 2 bits must be 0

  // DWORD 5
  uint32_t TransferCount;  // Number of bytes to transfer. Bit 0 must be 0

  // DWORD 6
  uint32_t resvd;  // Reserved
} __attribute__((packed));

struct PioSetupFis {
  // DWORD 0
  uint8_t fis_type;  // FIS_TYPE_PIO_SETUP

  uint8_t pmport : 4;  // Port multiplier
  uint8_t rsv0 : 1;    // Reserved
  uint8_t d : 1;       // Data transfer direction, 1 - device to host
  uint8_t i : 1;       // Interrupt bit
  uint8_t rsv1 : 1;

  uint8_t status;  // Status register
  uint8_t error;   // Error register

  // DWORD 1
  uint8_t lba0;    // LBA low register, 7:0
  uint8_t lba1;    // LBA mid register, 15:8
  uint8_t lba2;    // LBA high register, 23:16
  uint8_t device;  // Device register

  // DWORD 2
  uint8_t lba3;  // LBA register, 31:24
  uint8_t lba4;  // LBA register, 39:32
  uint8_t lba5;  // LBA register, 47:40
  uint8_t rsv2;  // Reserved

  // DWORD 3
  uint8_t countl;    // Count register, 7:0
  uint8_t counth;    // Count register, 15:8
  uint8_t rsv3;      // Reserved
  uint8_t e_status;  // New value of status register

  // DWORD 4
  uint16_t tc;      // Transfer count
  uint8_t rsv4[2];  // Reserved
} __attribute__((packed));

struct HostToDeviceRegisterFis {
  uint8_t fis_type;  // FIS_TYPE_REG_H2D
  uint8_t pmp_and_c;
  uint8_t command;   // Command register
  uint8_t featurel;  // Feature register, 7:0

  // DWORD 1
  uint8_t lba0;    // LBA low register, 7:0
  uint8_t lba1;    // LBA mid register, 15:8
  uint8_t lba2;    // LBA high register, 23:16
  uint8_t device;  // Device register

  // DWORD 2
  uint8_t lba3;      // LBA register, 31:24
  uint8_t lba4;      // LBA register, 39:32
  uint8_t lba5;      // LBA register, 47:40
  uint8_t featureh;  // Feature register, 15:8

  // DWORD 3
  uint16_t count;
  uint8_t icc;      // Isochronous command completion
  uint8_t control;  // Control register

  // DWORD 4
  uint32_t reserved;  // Reserved
} __attribute__((packed));
struct DeviceToHostRegisterFis {
  // DWORD 0
  uint8_t fis_type;  // FIS_TYPE_REG_D2H

  uint8_t pmport_and_i;

  uint8_t status;  // Status register
  uint8_t error;   // Error register

  // DWORD 1
  uint8_t lba0;    // LBA low register, 7:0
  uint8_t lba1;    // LBA mid register, 15:8
  uint8_t lba2;    // LBA high register, 23:16
  uint8_t device;  // Device register

  // DWORD 2
  uint8_t lba3;  // LBA register, 31:24
  uint8_t lba4;  // LBA register, 39:32
  uint8_t lba5;  // LBA register, 47:40
  uint8_t reserved1;

  // DWORD 3
  uint16_t count;
  uint16_t reserved2;

  uint32_t reserved3;
} __attribute__((packed));
struct SetDeviceBitsFis {
} __attribute__((packed));

struct ReceivedFis {
  DmaFis dma_fis;
  uint32_t reserved0;

  PioSetupFis pio_set_fis;
  uint32_t reserved1[3];

  DeviceToHostRegisterFis device_to_host_register_fis;
  uint32_t reserved2;

  SetDeviceBitsFis set_device_bits_fis;
  uint8_t unknown_fis[64];
} __attribute__((packed));
