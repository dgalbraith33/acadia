#include "boot/acpi.h"

#include <glacier/status/error.h>

#include "boot/boot_info.h"
#include "debug/debug.h"

#define K_ACPI_DEBUG 0

namespace {

static uint64_t gPcieEcBase = 0x0;
static uint64_t gPcieEcSize = 0x0;

static uint64_t gLApicBase = 0x0;
static uint64_t gIOApicBase = 0x0;

struct RsdpDescriptor {
  char signature[8];
  uint8_t checksum;
  char oem_id[6];
  uint8_t revision;
  uint32_t rsdt_addr;
  uint32_t length;
  uint64_t xsdt_addr;
  uint8_t ext_checksum;
  uint8_t reserved[3];
} __attribute__((packed));

struct SdtHeader {
  char signature[4];
  uint32_t length;
  uint8_t revision;
  uint8_t checksum;
  char oem_id[6];
  char oem_table_id[8];
  uint32_t oem_revision;
  uint32_t creator_id;
  uint32_t creator_revision;
} __attribute__((packed));

struct MadtEntry {
  uint8_t type;
  uint8_t length;
} __attribute__((packed));

struct MadtLocalApic {
  MadtEntry entry;
  uint8_t processor_id;
  uint8_t apic_id;
  uint32_t flags;
} __attribute__((packed));

struct MadtIoApic {
  MadtEntry entry;
  uint8_t io_apic_id;
  uint8_t reserved;
  uint32_t io_apic_address;
  uint32_t global_system_interrupt_base;
} __attribute__((packed));

struct MadtIoApicInterruptSource {
  MadtEntry entry;
  uint8_t bus_source;
  uint8_t irq_source;
  uint32_t global_system_interrupt;
  uint16_t flags;
} __attribute__((packed));

struct MadtLocalApicNonMaskable {
  MadtEntry entry;
  uint8_t apic_processor_id;
  uint16_t flags;
  uint8_t lint_num;
} __attribute__((packed));

struct MadtHeader {
  SdtHeader sdt_headr;
  uint32_t local_apic_address;
  uint32_t flags;
  MadtEntry first_entry;
} __attribute__((packed));

bool streq(const char* a, const char* b, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

bool checksum(uint8_t* addr, uint8_t num_bytes) {
  uint8_t check_cnt = 0;
  for (uint8_t i = 0; i < num_bytes; i++) {
    check_cnt += addr[i];
  }
  return check_cnt == 0;
}

void dbgsz(const char* c, uint8_t cnt) {
  char cl[cnt + 1];

  for (uint8_t i = 0; i < cnt; i++) {
    cl[i] = c[i];
  }
  cl[cnt] = '\0';
  dbgln(cl);
}

uint8_t* SdtDataStart(SdtHeader* sdt) {
  return reinterpret_cast<uint8_t*>(sdt) + sizeof(SdtHeader);
}

void ParseMcfg(SdtHeader* rsdt) {
#if K_ACPI_DEBUG
  dbgsz(rsdt->signature, 4);
#endif
  uint32_t size = (rsdt->length - sizeof(SdtHeader)) / 16;
  uint64_t* entries = reinterpret_cast<uint64_t*>(SdtDataStart(rsdt));
  // There are 8 reserved bytes at the end of the table.
  entries++;
  for (uint32_t i = 0; i < size; i++) {
    uint64_t base_ecm_addr = entries[2 * i];
    uint64_t bus_info = entries[2 * i + 1];
    if (bus_info != 0xff000000) {
#if K_ACPI_DEBUG
      dbgln("WARN: Unexpected bus-info for PCI EC. Mem region will be wrong.");
#endif
    }
    gPcieEcBase = base_ecm_addr;
    uint64_t num_busses = 0x100;
    uint64_t dev_per_bus = 0x20;
    uint64_t fns_per_dev = 0x8;
    uint64_t bytes_per_fn = 0x1000;
    gPcieEcSize = num_busses * dev_per_bus * fns_per_dev * bytes_per_fn;
#if K_ACPI_DEBUG
    dbgln("PCI Map: {x}:{x}", gPcieEcBase, gPcieEcSize);
#endif
  }
}

void ParseMadt(SdtHeader* rsdt) {
#if K_ACPI_DEBUG
  dbgsz(rsdt->signature, 4);
#endif
  uint64_t max_addr = reinterpret_cast<uint64_t>(rsdt) + rsdt->length;
  MadtHeader* header = reinterpret_cast<MadtHeader*>(rsdt);

#if K_ACPI_DEBUG
  dbgln("Local APIC {x}", +header->local_apic_address);
  dbgln("Flags: {x}", +header->flags);
#endif

  gLApicBase = header->local_apic_address;

  MadtEntry* entry = &header->first_entry;

  while (reinterpret_cast<uint64_t>(entry) < max_addr) {
    switch (entry->type) {
      case 0: {
        MadtLocalApic* local = reinterpret_cast<MadtLocalApic*>(entry);
#if K_ACPI_DEBUG
        dbgln("Local APIC (Proc id, id, flags): {x}, {x}, {x}",
              local->processor_id, local->apic_id, +local->flags);
#endif
        break;
      }
      case 1: {
        MadtIoApic* io = reinterpret_cast<MadtIoApic*>(entry);
#if K_ACPI_DEBUG
        dbgln("IO Apic (id, addr, gsi base): {x}, {x}, {x}", io->io_apic_id,
              +io->io_apic_address, +io->global_system_interrupt_base);
#endif
        if (gIOApicBase != 0) {
          dbgln("More than one IOApic, unhandled");
        }
        gIOApicBase = io->io_apic_address;
        break;
      }
      case 2: {
        MadtIoApicInterruptSource* src =
            reinterpret_cast<MadtIoApicInterruptSource*>(entry);
#if K_ACPI_DEBUG
        dbgln("IO Source (Bus, IRQ, GSI, flags): {x}, {x}, {x}, {x}",
              src->bus_source, src->irq_source, +src->global_system_interrupt,
              +src->flags);
#endif
        break;
      }
      case 4: {
        MadtLocalApicNonMaskable* lnmi =
            reinterpret_cast<MadtLocalApicNonMaskable*>(entry);
#if K_ACPI_DEBUG
        dbgln("Local NMI (proc id, flags, lint#): {x}, {x}, {x}",
              lnmi->apic_processor_id, +lnmi->flags, lnmi->lint_num);
#endif
        break;
      }
      default:
        dbgln("Unhandled entry type: {}", entry->type);
    }
    entry = reinterpret_cast<MadtEntry*>(reinterpret_cast<uint64_t>(entry) +
                                         entry->length);
  }
}

void ParseSdt(SdtHeader* rsdt) {
  if (!checksum((uint8_t*)rsdt, rsdt->length)) {
    dbgln("Bad RSDT checksum.");
    return;
  }
  if (streq(rsdt->signature, "MCFG", 4)) {
    ParseMcfg(rsdt);
  } else if (streq(rsdt->signature, "APIC", 4)) {
    ParseMadt(rsdt);
  } else {
#if K_ACPI_DEBUG
    dbgln("Unhandled:");
    dbgsz(rsdt->signature, 4);
#endif
  }
}

void ProbeRsdt(SdtHeader* rsdt) {
  if (!checksum((uint8_t*)rsdt, rsdt->length)) {
    dbgln("Bad RSDT checksum.");
    return;
  }
#if K_ACPI_DEBUG
  dbgsz(rsdt->signature, 4);
#endif
  uint32_t size = (rsdt->length - sizeof(SdtHeader)) / 4;
  uint32_t* entries = reinterpret_cast<uint32_t*>(SdtDataStart(rsdt));
  for (uint32_t i = 0; i < size; i++) {
    SdtHeader* table = reinterpret_cast<SdtHeader*>(entries[i]);
    ParseSdt(table);
  }
}

}  // namespace

void ProbeRsdp() {
  void* rsdp_addr = boot::GetRsdpAddr();
  RsdpDescriptor* rsdp = static_cast<RsdpDescriptor*>(rsdp_addr);
  if (!streq(rsdp->signature, "RSD PTR ", 8)) {
    dbgln("Invalid Rsdp!");
    return;
  }

  // Checks V1 up to rsdt_addr.
  if (!checksum((uint8_t*)rsdp_addr, 20)) {
    dbgln("Rsdp Check failed");
    return;
  }

#if K_ACPI_DEBUG
  dbgln("ACPI Ver {}", rsdp->revision);
  dbgln("RSDT Addr: {x}", +rsdp->rsdt_addr);
#endif

  ProbeRsdt(reinterpret_cast<SdtHeader*>(rsdp->rsdt_addr));

  if (rsdp->revision == 0) {
    return;
  }

  // Checks V2 (entire structure).
  if (!checksum((uint8_t*)rsdp_addr, rsdp->length)) {
    dbgln("Rsdp ext checksum failed");
    return;
  }

#if K_ACPI_DEBUG
  dbgln("XSDT Addr: {x}", +rsdp->xsdt_addr);
#endif
}

glcr::ErrorOr<PcieConfiguration> GetPciExtendedConfiguration() {
  if (gPcieEcSize == 0) {
    return glcr::NOT_FOUND;
  }

  return PcieConfiguration{gPcieEcBase, gPcieEcSize};
}

glcr::ErrorOr<ApicConfiguration> GetApicConfiguration() {
  if (gLApicBase == 0 || gIOApicBase == 0) {
    return glcr::NOT_FOUND;
  }

  return ApicConfiguration{gLApicBase, gIOApicBase};
}
