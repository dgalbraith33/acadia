#pragma once

#include <stdint.h>

void InspectApic();

void EnableApic();

#define LAPIC_TIMER_ONESHOT 0
#define LAPIC_TIMER_PERIODIC 1 << 17

void SetLocalTimer(uint32_t init_cnt, uint64_t mode);
uint32_t GetLocalTimer();

void UnmaskPit();
void MaskPit();

void SignalEOI();
