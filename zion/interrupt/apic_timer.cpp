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
  gApic->InitializeLocalTimer(0xFFFFFFFF, Apic::ONESHOT);
  gApic->UnmaskPit();
}

void ApicTimer::Calibrate() {
  if (calibration_.initial_measurement == 0) {
    calibration_.initial_measurement = gApic->GetLocalTimerValue();
    return;
  }
  calibration_.tick_count++;

  if (calibration_.tick_count == 10) {
    calculated_frequency_ =
        10 * (calibration_.initial_measurement - gApic->GetLocalTimerValue());
    FinishCalibration();
  }
}

void ApicTimer::FinishCalibration() {
  gApic->MaskPit();
  gApic->InitializeLocalTimer(calculated_frequency_ / kScheduleFrequency,
                              Apic::PERIODIC);
}
