#pragma once

#include <stdint.h>

#include "include/ztypes.h"

void ProbeRsdp();

z_err_t GetPciExtendedConfiguration(uint64_t* base, uint64_t* offset);
