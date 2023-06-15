#pragma once

#include <mammoth/thread.h>
#include <zerrors.h>

#include "ahci/ahci.h"
#include "ahci/ahci_device.h"

class AhciDriver {
 public:
  z_err_t Init();

  void InterruptLoop();

  z_err_t GetDevice(uint64_t id, AhciDevice& device);

  void DumpCapabilities();
  void DumpPorts();

 private:
  MappedMemoryRegion pci_region_;
  PciDeviceHeader* pci_device_header_ = nullptr;
  MappedMemoryRegion ahci_region_;
  AhciHba* ahci_hba_ = nullptr;

  // TODO: Allocate these dynamically.
  AhciDevice devices_[32];

  Thread irq_thread_;
  uint64_t irq_port_cap_ = 0;

  uint64_t num_ports_;
  uint64_t num_commands_;

  z_err_t LoadPciDeviceHeader();
  z_err_t LoadCapabilities();
  z_err_t RegisterIrq();
  z_err_t LoadHbaRegisters();
  z_err_t LoadDevices();
};
