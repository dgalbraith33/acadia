#pragma once

#include <stdint.h>
#include <ztypes.h>

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

// TODO: Figure out if it safe to hardcode this.
// For the memory mapped access to PCI, it may be true that
// each configuration item is always the size of a single page.
const uint64_t kPcieConfigurationSize = 0x1000;

class PciReader {
 public:
  PciReader();

  z_cap_t GetAhciVmmo();
  z_cap_t GetXhciVmmo();

 private:
  PciDeviceHeader* header_;

  uint64_t achi_device_offset_ = 0;
  uint64_t xhci_device_offset_ = 0;

  void PciDump(uint64_t vaddr);
  void BusDump(uint64_t base, uint64_t bus);
  void DeviceDump(uint64_t base, uint64_t bus, uint64_t dev);
  void FunctionDump(uint64_t base, uint64_t bus, uint64_t dev, uint64_t fun);
};
