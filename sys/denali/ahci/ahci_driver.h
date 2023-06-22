#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/thread.h>
#include <ztypes.h>

#include "ahci/ahci.h"
#include "ahci/ahci_device.h"

class AhciDriver {
 public:
  glcr::ErrorCode Init();

  void InterruptLoop();

  glcr::ErrorOr<AhciDevice*> GetDevice(uint64_t id);

  void DumpCapabilities();
  void DumpPorts();

 private:
  MappedMemoryRegion pci_region_;
  PciDeviceHeader* pci_device_header_ = nullptr;
  MappedMemoryRegion ahci_region_;
  AhciHba* ahci_hba_ = nullptr;

  // TODO: Allocate these dynamically.
  AhciDevice* devices_[32];

  Thread irq_thread_;
  uint64_t irq_port_cap_ = 0;

  uint64_t num_ports_;
  uint64_t num_commands_;

  glcr::ErrorCode LoadPciDeviceHeader();
  glcr::ErrorCode LoadCapabilities();
  glcr::ErrorCode RegisterIrq();
  glcr::ErrorCode LoadHbaRegisters();
  glcr::ErrorCode LoadDevices();
};
