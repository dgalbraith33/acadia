#include "hw/pcie.h"

#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <zcall.h>

namespace {

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

PciDeviceHeader* PciHeader(uint64_t base, uint64_t bus, uint64_t dev,
                           uint64_t fun) {
  return reinterpret_cast<PciDeviceHeader*>(base + (bus << 20) + (dev << 15) +
                                            (fun << 12));
}

void FunctionDump(uint64_t base, uint64_t bus, uint64_t dev, uint64_t fun) {
  PciDeviceHeader* hdr = PciHeader(base, bus, dev, fun);
  if (hdr->vendor_id == 0xFFFF) {
    return;
  }
  dbgln(
      "[%u.%u.%u] (Vendor, Device): (%x, %x), (Type, Class, Sub, PIF): (%u, "
      "%x, %x, %x)",
      bus, dev, fun, hdr->vendor_id, hdr->device_id, hdr->header_type,
      hdr->class_code, hdr->subclass, hdr->prog_interface);

  if ((hdr->class_code == 0x6) && (hdr->subclass == 0x4)) {
    dbgln("FIXME: Handle PCI to PCI bridge.");
  }
  if (hdr->class_code == 0x1) {
    dbgln("SATA Device at: %lx", reinterpret_cast<uint64_t>(hdr) - base);
  }
}

void DeviceDump(uint64_t base, uint64_t bus, uint64_t dev) {
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

void BusDump(uint64_t base, uint64_t bus) {
  for (uint64_t dev = 0; dev < 0x20; dev++) {
    DeviceDump(base, bus, dev);
  }
}

void PciDump(uint64_t base) {
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

}  // namespace

void DumpPciEDevices() {
  dbgln("Creating PCI obj");
  uint64_t vmmo_cap, vmmo_size;
  check(ZTempPcieConfigObjectCreate(&vmmo_cap, &vmmo_size));

  dbgln("Creating addr space");
  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));
  dbgln("Addr %lx", vaddr);

  dbgln("Dumping PCI");
  PciDump(vaddr);
  dbgln("Done");
}
