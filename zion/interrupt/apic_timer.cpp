#include "interrupt/apic_timer.h"

#include "debug/debug.h"
#include "interrupt/apic.h"
#include "interrupt/timer.h"

const uint32_t kScheduleFrequency = 20;
ApicTimer* gApicTimer = nullptr;

void ApicTimer::Init() {
  gApicTimer = new ApicTimer();
  gApicTimer->StartCalibration();
  asm("sti;");
  gApicTimer->WaitCalibration();
  asm("cli;");
}

void ApicTimer::StartCalibration() {
  SetFrequency(100);
  SetLocalTimer(0xFFFFFFFF, 0);
  UnmaskPit();
}

void ApicTimer::Calibrate() {
  if (calibration_.initial_measurement == 0) {
    calibration_.initial_measurement = GetLocalTimer();
    return;
  }
  calibration_.tick_count++;

  if (calibration_.tick_count == 10) {
    calculated_frequency_ =
        10 * (calibration_.initial_measurement - GetLocalTimer());
    FinishCalibration();
  }
}

void ApicTimer::FinishCalibration() {
  MaskPit();
  SetLocalTimer(calculated_frequency_ / kScheduleFrequency,
                LAPIC_TIMER_PERIODIC);
}
