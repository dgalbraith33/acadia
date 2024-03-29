#include "hw/pcie.h"

#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <zcall.h>

#define PCI_DEBUG 1

namespace {

PciDeviceHeader* PciHeader(uint64_t base, uint64_t bus, uint64_t dev,
                           uint64_t fun) {
  return reinterpret_cast<PciDeviceHeader*>(base + (bus << 20) + (dev << 15) +
                                            (fun << 12));
}

}  // namespace

PciReader::PciReader() {
  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootPciVmmoCap, 0, &vaddr));

  PciDump(vaddr);

  header_ = PciHeader(vaddr, 0, 0, 0);
}

z_cap_t PciReader::GetAhciVmmo() {
  uint64_t new_cap;
  check(ZMemoryObjectDuplicate(gBootPciVmmoCap, achi_device_offset_,
                               kPcieConfigurationSize, &new_cap));
  return new_cap;
}

z_cap_t PciReader::GetXhciVmmo() {
  uint64_t new_cap;
  check(ZMemoryObjectDuplicate(gBootPciVmmoCap, xhci_device_offset_,
                               kPcieConfigurationSize, &new_cap));
  return new_cap;
}

void PciReader::FunctionDump(uint64_t base, uint64_t bus, uint64_t dev,
                             uint64_t fun) {
  PciDeviceHeader* hdr = PciHeader(base, bus, dev, fun);
  if (hdr->vendor_id == 0xFFFF) {
    return;
  }
#if PCI_DEBUG
  dbgln(
      "[{}.{}.{}] (Vendor, Device): ({x}, {x}), (Type, Class, Sub, PIF): ({}, "
      "{x}, {x}, {x})",
      bus, dev, fun, hdr->vendor_id, hdr->device_id, hdr->header_type,
      hdr->class_code, hdr->subclass, hdr->prog_interface);
#endif

  if ((hdr->class_code == 0x6) && (hdr->subclass == 0x4)) {
    dbgln("FIXME: Handle PCI to PCI bridge.");
  }
  if (hdr->class_code == 0x1) {
#if PCI_DEBUG
    dbgln("SATA Device at: {x}", reinterpret_cast<uint64_t>(hdr) - base);
#endif
    achi_device_offset_ = reinterpret_cast<uint64_t>(hdr) - base;
  }

  if (hdr->class_code == 0xC && hdr->subclass == 0x3) {
    if (hdr->prog_interface == 0x30) {
      xhci_device_offset_ = reinterpret_cast<uint64_t>(hdr) - base;
    } else {
      dbgln("WARN: Non-XHCI USB Controller found");
    }
  }
}

void PciReader::DeviceDump(uint64_t base, uint64_t bus, uint64_t dev) {
  PciDeviceHeader* hdr = PciHeader(base, bus, dev, 0);
  if (hdr->vendor_id == 0xFFFF) {
    return;
  }

  FunctionDump(base, bus, dev, 0);

  // Device is multifunction.
  if (hdr->header_type & 0x80) {
    for (uint64_t f = 1; f < 0x8; f++) {
      FunctionDump(base, bus, dev, f);
    }
  }
}

void PciReader::BusDump(uint64_t base, uint64_t bus) {
  for (uint64_t dev = 0; dev < 0x20; dev++) {
    DeviceDump(base, bus, dev);
  }
}

void PciReader::PciDump(uint64_t base) {
  PciDeviceHeader* hdr = PciHeader(base, 0, 0, 0);
  if ((hdr->header_type & 0x80) == 0) {
    // Single bus system.
    BusDump(base, 0);
  } else {
    for (uint64_t f = 0; f < 8; f++) {
      PciDeviceHeader* f_hdr = PciHeader(base, 0, 0, f);
      if (f_hdr->vendor_id != 0xFFFF) {
        BusDump(base, f);
      }
    }
  }
}
