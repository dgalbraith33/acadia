#pragma once

#include <glacier/status/error_or.h>
#include <stdint.h>

#include "include/ztypes.h"

void ProbeRsdp();

struct PcieConfiguration {
  uint64_t base;
  uint64_t offset;
};
glcr::ErrorOr<PcieConfiguration> GetPciExtendedConfiguration();

struct ApicConfiguration {
  uint64_t lapic_base;
  uint64_t ioapic_base;
};
glcr::ErrorOr<ApicConfiguration> GetApicConfiguration();
