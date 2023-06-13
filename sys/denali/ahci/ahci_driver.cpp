#include "ahci/ahci_driver.h"

#include <mammoth/debug.h>
#include <stdint.h>
#include <zcall.h>

namespace {

const uint64_t kSataPciPhys = 0xB00FA000;
const uint64_t kPciSize = 0x1000;

const uint64_t kGhc_InteruptEnable = 0x2;

void interrupt_thread(void* void_driver) {
  AhciDriver* driver = static_cast<AhciDriver*>(void_driver);

  driver->InterruptLoop();

  crash("Driver returned from interrupt loop", Z_ERR_UNIMPLEMENTED);
}

}  // namespace

z_err_t AhciDriver::Init() {
  RET_ERR(LoadPciDeviceHeader());
  // RET_ERR(LoadCapabilities());
  RET_ERR(RegisterIrq());
  RET_ERR(LoadHbaRegisters());
  ahci_hba_->global_host_control |= kGhc_InteruptEnable;
  RET_ERR(LoadDevices());
  // DumpCapabilities();
  // DumpPorts();
  return Z_OK;
}

void AhciDriver::DumpCapabilities() {
  dbgln("AHCI Capabilities:");
  uint32_t caps = ahci_hba_->capabilities;

  if (caps & 0x20) {
    dbgln("External SATA");
  }
  if (caps & 0x40) {
    dbgln("Enclosure Management");
  }
  if (caps & 0x80) {
    dbgln("Command Completion Coalescing");
  }
  if (caps & 0x2000) {
    dbgln("Partial State Capable");
  }
  if (caps & 0x4000) {
    dbgln("Slumber state capable");
  }
  if (caps & 0x8000) {
    dbgln("PIO Multiple DRQ Block");
  }
  if (caps & 0x1'0000) {
    dbgln("FIS-Based Switching");
  }
  if (caps & 0x2'0000) {
    dbgln("Port Multiplier");
  }
  if (caps & 0x4'0000) {
    dbgln("AHCI mode only");
  }
  dbgln("Speed support: %u", (caps & 0xF0'0000) >> 20);
  if (caps & 0x100'0000) {
    dbgln("Command list override");
  }
  if (caps & 0x200'0000) {
    dbgln("Activity LED");
  }
  if (caps & 0x400'0000) {
    dbgln("Aggresive link power management");
  }
  if (caps & 0x800'0000) {
    dbgln("Staggered spin up");
  }
  if (caps & 0x1000'0000) {
    dbgln("Mechanical Switch Presence");
  }
  if (caps & 0x2000'0000) {
    dbgln("SNotification Register");
  }
  if (caps & 0x4000'0000) {
    dbgln("Native Command Queueing");
  }
  if (caps & 0x8000'0000) {
    dbgln("64bit Addressing");
  }

  // Secondary.
  caps = ahci_hba_->capabilities_ext;
  if (caps & 0x1) {
    dbgln("BIOS/OS handoff");
  }
  if (caps & 0x2) {
    dbgln("NVMHCI Present");
  }
  if (caps & 0x4) {
    dbgln("Auto partial to slumber tranisitions");
  }
  if (caps & 0x8) {
    dbgln("Device sleep");
  }
  if (caps & 0x10) {
    dbgln("Aggressive device sleep management");
  }

  dbgln("Control %x", ahci_hba_->global_host_control);
}

void AhciDriver::DumpPorts() {
  for (uint64_t i = 0; i < 6; i++) {
    AhciDevice& dev = devices_[i];
    if (!dev.IsInit()) {
      continue;
    }

    dbgln("");
    dbgln("Port %u:", i);
    dev.DumpInfo();
  }
}

void AhciDriver::InterruptLoop() {
  while (true) {
    uint64_t type, bytes, caps;
    check(ZPortRecv(irq_port_cap_, 0, 0, 0, 0, &type, &bytes, &caps));
    for (uint64_t i = 0; i < 32; i++) {
      if (devices_[i].IsInit() && (ahci_hba_->interrupt_status & (1 << i))) {
        devices_[i].HandleIrq();
        ahci_hba_->interrupt_status &= ~(1 << i);
      }
    }
  }
}

z_err_t AhciDriver::LoadPciDeviceHeader() {
  pci_region_ = MappedMemoryRegion::DirectPhysical(kSataPciPhys, kPciSize);
  pci_device_header_ = reinterpret_cast<PciDeviceHeader*>(pci_region_.vaddr());
  return Z_OK;
}

z_err_t AhciDriver::LoadCapabilities() {
  if (!(pci_device_header_->status_reg & 0x10)) {
    dbgln("No caps!");
    return Z_ERR_INVALID;
  }
  uint8_t* base = reinterpret_cast<uint8_t*>(pci_device_header_);
  uint16_t offset = pci_device_header_->cap_ptr;
  do {
    uint16_t* cap = reinterpret_cast<uint16_t*>(base + offset);
    switch (*cap & 0xFF) {
      case 0x01:
        dbgln("Power Management");
        break;
      case 0x05:
        dbgln("MSI");
        break;
      case 0x12:
        dbgln("SATA");
        break;
      default:
        dbgln("Unrecognized cap");
        break;
    }

    offset = (*cap & 0xFF00) >> 8;
  } while (offset);
  return Z_OK;
}

z_err_t AhciDriver::RegisterIrq() {
  if (pci_device_header_->interrupt_pin == 0) {
    crash("Can't register IRQ without a pin num", Z_INVALID);
  }
  uint64_t irq_num = Z_IRQ_PCI_BASE + pci_device_header_->interrupt_pin - 1;
  RET_ERR(ZIrqRegister(irq_num, &irq_port_cap_));
  irq_thread_ = Thread(interrupt_thread, this);
  return Z_OK;
}

z_err_t AhciDriver::LoadHbaRegisters() {
  ahci_region_ =
      MappedMemoryRegion::DirectPhysical(pci_device_header_->abar, 0x1100);
  ahci_hba_ = reinterpret_cast<AhciHba*>(ahci_region_.vaddr());
  num_ports_ = (ahci_hba_->capabilities & 0x1F) + 1;
  num_commands_ = ((ahci_hba_->capabilities & 0x1F00) >> 8) + 1;

  return Z_OK;
}

z_err_t AhciDriver::LoadDevices() {
  for (uint8_t i = 0; i < 32; i++) {
    if (!(ahci_hba_->port_implemented & (1 << i))) {
      continue;
    }
    uint64_t port_addr =
        reinterpret_cast<uint64_t>(ahci_hba_) + 0x100 + (0x80 * i);
    devices_[i] = AhciDevice(reinterpret_cast<AhciPort*>(port_addr));
  }
  return Z_OK;
}
