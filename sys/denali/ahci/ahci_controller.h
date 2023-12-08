#pragma once

#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "ahci/ahci.h"
#include "ahci/ahci_device.h"

class AhciController {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<AhciController>> Init(
      mmth::OwnedMemoryRegion&& ahci_phys);
  glcr::ErrorCode RegisterIrq();

  void InterruptLoop();

  glcr::ErrorOr<AhciDevice*> GetDevice(uint64_t id);

  void DumpCapabilities();
  void DumpPorts();

 private:
  mmth::OwnedMemoryRegion pci_region_;
  PciDeviceHeader* pci_device_header_ = nullptr;
  mmth::OwnedMemoryRegion ahci_region_;
  volatile AhciHba* ahci_hba_ = nullptr;

  glcr::UniquePtr<AhciDevice> devices_[32];

  Thread irq_thread_;
  uint64_t irq_port_cap_ = 0;

  uint8_t num_ports_;
  uint8_t num_commands_;

  glcr::ErrorCode LoadCapabilities();
  glcr::ErrorCode LoadHbaRegisters();
  glcr::ErrorCode ResetHba();
  glcr::ErrorCode LoadDevices();

  AhciController(mmth::OwnedMemoryRegion&& pci_region)
      : pci_region_(glcr::Move(pci_region)),
        pci_device_header_(
            reinterpret_cast<PciDeviceHeader*>(pci_region_.vaddr())) {}
};
