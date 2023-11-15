#pragma once

#include <stdint.h>

const uint64_t KiB = 0x400;
const uint64_t kPageSize = 4 * KiB;

const uint64_t kKernelSlabHeapStart = 0xFFFF'FFFF'4000'0000;
const uint64_t kKernelSlabHeapEnd = 0xFFFF'FFFF'6000'0000;
const uint64_t kKernelBuddyHeapStart = 0xFFFF'FFFF'6000'0000;
const uint64_t kKernelBuddyHeapEnd = 0xFFFF'FFFF'8000'0000;

// Note the kernel code isn't actually this large, we just reserve a lot of
// address space for it. (256 MiB).
const uint64_t kKernelCodeStart = 0xFFFF'FFFF'8000'0000;
const uint64_t kKernelCodeEnd = 0xFFFF'FFFF'9000'0000;

const uint64_t kKernelStackStart = 0xFFFF'FFFF'9000'0000;
const uint64_t kKernelStackEnd = 0xFFFF'FFFF'A000'0000;

const uint64_t kKernelStackSize = 3 * kPageSize;
const uint64_t kKernelStackOffset = 4 * kPageSize;
