#pragma once

#include <stdint.h>
#include <ztypes.h>

z_err_t CheckMbrIsGpt(uint64_t mem_cap);

z_err_t ReadPartitionHeader(uint64_t mem_cap);

z_err_t ReadPartitionEntries(uint64_t mem_cap);
