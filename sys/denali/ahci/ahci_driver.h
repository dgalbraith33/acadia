#pragma once

#include <zerrors.h>

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
  uint32_t subsystem_id;
  uint32_t expansion_rom;
  uint8_t cap_ptr;
  uint8_t reserved[7];
  uint8_t interrupt_line;
  uint8_t interrupt_pin;
  uint8_t min_grant;
  uint8_t max_latency;
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
};

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
};

class AhciDriver {
 public:
  z_err_t Init();

  void DumpCapabilities();
  void DumpPorts();

 private:
  PciDeviceHeader* pci_device_header_ = nullptr;
  AhciHba* ahci_hba_ = nullptr;

  z_err_t LoadPciDeviceHeader();
  z_err_t LoadHbaRegisters();
};
