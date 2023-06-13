#pragma once

#include <mammoth/thread.h>
#include <zerrors.h>

#include "ahci/ahci.h"
#include "ahci/ahci_device.h"

class AhciDriver {
 public:
  z_err_t Init();

  void InterruptLoop();

  void DumpCapabilities();
  void DumpPorts();

 private:
  PciDeviceHeader* pci_device_header_ = nullptr;
  AhciHba* ahci_hba_ = nullptr;

  AhciDevice devices_[6];

  Thread irq_thread_;
  uint64_t irq_port_cap_ = 0;

  z_err_t LoadPciDeviceHeader();
  z_err_t LoadCapabilities();
  z_err_t RegisterIrq();
  z_err_t LoadHbaRegisters();
  z_err_t LoadDevices();
};
