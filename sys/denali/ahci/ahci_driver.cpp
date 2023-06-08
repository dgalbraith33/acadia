#include "ahci/ahci_driver.h"

#include <mammoth/debug.h>
#include <stdint.h>
#include <zcall.h>

namespace {

const uint64_t kSataPciPhys = 0xB00FA000;
const uint64_t kPciSize = 0x1000;

}  // namespace

z_err_t AhciDriver::Init() {
  RET_ERR(LoadPciDeviceHeader());
  dbgln("ABAR: %x", pci_device_header_->abar);
  RET_ERR(LoadHbaRegisters());
  dbgln("Version: %x", ahci_hba_->version);
  DumpCapabilities();
  DumpPorts();
  return Z_OK;
}

void AhciDriver::DumpCapabilities() {
  dbgln("AHCI Capabilities:");
  uint32_t caps = ahci_hba_->capabilities;

  dbgln("Num Ports: %u", (caps & 0x1F) + 1);
  dbgln("Num Command Slots: %u", (caps & 0x1F00) >> 8);
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
}

void AhciDriver::DumpPorts() {
  dbgln("Ports implemented %x", ahci_hba_->port_implemented);

  uint64_t port_index = 0;
  uint32_t ports_implemented = ahci_hba_->port_implemented;
  while (ports_implemented) {
    if (!(ports_implemented & 0x1)) {
      ports_implemented >>= 1;
      port_index++;
      continue;
    }
    uint64_t port_addr =
        reinterpret_cast<uint64_t>(ahci_hba_) + 0x100 + (0x80 * port_index);
    AhciPort* port = reinterpret_cast<AhciPort*>(port_addr);

    dbgln("");
    dbgln("Port %u:", port_index);
    dbgln("Comlist: %lx", port->command_list_base);
    dbgln("FIS: %lx", port->fis_base);
    dbgln("Command: %x", port->command);
    dbgln("Signature: %x", port->signature);
    dbgln("SATA status: %x", port->sata_status);

    ports_implemented >>= 1;
    port_index++;
  }
}

z_err_t AhciDriver::LoadPciDeviceHeader() {
  uint64_t vmmo_cap;
  RET_ERR(ZMemoryObjectCreatePhysical(kSataPciPhys, kPciSize, &vmmo_cap));

  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, vmmo_cap, &vaddr));
  pci_device_header_ = reinterpret_cast<PciDeviceHeader*>(vaddr);
  return Z_OK;
}

z_err_t AhciDriver::LoadHbaRegisters() {
  uint64_t vmmo_cap;
  RET_ERR(
      ZMemoryObjectCreatePhysical(pci_device_header_->abar, 0x1100, &vmmo_cap));

  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, vmmo_cap, &vaddr));
  ahci_hba_ = reinterpret_cast<AhciHba*>(vaddr);
  return Z_OK;
}
