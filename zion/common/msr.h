#pragma once

#include <stdint.h>

uint64_t GetMSR(uint32_t msr);
void SetMSR(uint32_t msr, uint64_t val);
