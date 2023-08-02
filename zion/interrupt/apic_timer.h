#pragma once

#include <stdint.h>

class ApicTimer {
 public:
  static void Init();

  // FIXME: The calibration is not currently very accurate due to time taken
  // handling the PIT interrupts. It would probably be good to revisit this
  // after implementing HPET.
  void Calibrate();

  void WaitCalibration() {
    while (calculated_frequency_ == 0) {
      asm("hlt;");
    }
  }

 private:
  struct Calibration {
    uint32_t initial_measurement = 0;
    uint32_t tick_count = 0;
  };
  Calibration calibration_;

  uint64_t calculated_frequency_ = 0;

  ApicTimer() {}
  void StartCalibration();
  void FinishCalibration();
};

extern ApicTimer* gApicTimer;
