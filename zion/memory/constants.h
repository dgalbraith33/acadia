#pragma once

#include <stdint.h>

const uint64_t KiB = 0x400;
const uint64_t kPageSize = 4 * KiB;

const uint64_t kKernelSlabHeapStart = 0xFFFF'FFFF'4000'0000;
const uint64_t kKernelSlabHeapEnd = 0xFFFF'FFFF'6000'0000;
const uint64_t kKernelBuddyHeapStart = 0xFFFF'FFFF'6000'0000;
const uint64_t kKernelBuddyHeapEnd = 0xFFFF'FFFF'8000'0000;
