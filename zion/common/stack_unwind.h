#pragma once

#include <stdint.h>

const uint64_t kStackBaseSentinel = 0xABBACDCD'12345678;

void StackUnwind(uint64_t rbp);
