#include "hw/pcie.h"

#include <mammoth/debug.h>
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

void DumpFn(uint64_t fn_start, uint64_t bus, uint64_t dev, uint64_t fun) {
  PciDeviceHeader* hdr = reinterpret_cast<PciDeviceHeader*>(fn_start);
  if (hdr->vendor_id == 0xFFFF) {
    return;
  }
  dbgln(
      "[%u.%u.%u] (Vendor, Device): (%x, %x), (Type, Class, Sub, PIF): (%u, "
      "%x, %x, %x)",
      bus, dev, fun, hdr->vendor_id, hdr->device_id, hdr->header_type,
      hdr->class_code, hdr->subclass, hdr->prog_interface);
}

void PciDump(uint64_t base, uint64_t size) {
  for (uint64_t b = 0; b <= 0xFF; b++) {
    for (uint64_t d = 0; d <= 0x1F; d++) {
      for (uint64_t f = 0; f < 8; f++) {
        uint64_t fn_base = base + (b << 20) + (d << 15) + (f << 12);
        DumpFn(fn_base, b, d, f);
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
  check(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, vmmo_cap, &vaddr));
  dbgln("Addr %x", vaddr);

  dbgln("Dumping PCI");
  PciDump(vaddr, vmmo_size);
  dbgln("Done");
}
