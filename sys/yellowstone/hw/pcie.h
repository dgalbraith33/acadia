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
} __attribute__((packed));

class PciReader {
 public:
  PciReader();

  uint64_t GetAhciPhysical();

 private:
  PciDeviceHeader* header_;
  // FIXME: Don't hardcode this (It is available from ACPI).
  uint64_t phys_mem_offset_ = 0xB000'0000;

  uint64_t achi_device_offset_ = 0;

  void PciDump(uint64_t vaddr);
  void BusDump(uint64_t base, uint64_t bus);
  void DeviceDump(uint64_t base, uint64_t bus, uint64_t dev);
  void FunctionDump(uint64_t base, uint64_t bus, uint64_t dev, uint64_t fun);
};
