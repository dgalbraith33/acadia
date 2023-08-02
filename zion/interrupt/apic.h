#pragma once

#include <stdint.h>

#include "boot/acpi.h"

class Apic {
 public:
  static void Init();
  static void DumpInfo();

  uint32_t GetLocalTimerValue();
  enum TimerMode {
    ONESHOT,
    PERIODIC,
  };
  void InitializeLocalTimer(uint32_t init_cnt, TimerMode mode);

  void SignalEOI();

  void UnmaskPit();
  void MaskPit();

 private:
  uint64_t l_apic_base_;
  volatile uint8_t* io_apic_addr_;
  volatile uint32_t* io_apic_data_;

  Apic(const ApicConfiguration& config);

  uint32_t GetLocalReg(uint16_t offset);
  void SetLocalReg(uint16_t offset, uint32_t value);

  uint64_t GetIoDoubleReg(uint8_t offset);
  void SetIoDoubleReg(uint8_t offset, uint64_t value);

  uint32_t GetIoReg(uint8_t offset);
};

extern Apic* gApic;
