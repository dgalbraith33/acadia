#include "interrupt/apic.h"

#include <stdint.h>

#include "boot/boot_info.h"
#include "common/msr.h"
#include "common/port.h"
#include "debug/debug.h"
#include "interrupt/apic_timer.h"

#define APIC_DEBUG 0

namespace {

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100  // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

#define LAPIC_TIMER_ONESHOT 0
#define LAPIC_TIMER_PERIODIC 1 << 17

#define APIC_MASK 0x10000

const uint16_t kEoiOffset = 0xB0;
const uint16_t kLvtTimerOffset = 0x320;
const uint16_t kTimerInitOffset = 0x380;
const uint16_t kTimerCurrOffset = 0x390;
const uint16_t kTimerDivOffset = 0x3E0;

uint32_t volatile* GetPhys(uint64_t base, uint64_t offset = 0) {
  return reinterpret_cast<uint32_t*>(boot::GetHigherHalfDirectMap() + base +
                                     offset);
}

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC1_DATA 0x21
#define PIC2_DATA 0xA1

void MaskPic() {
  outb(PIC1_DATA, 0xFF);
  outb(PIC2_DATA, 0xFF);
  outb(PIC1_COMMAND, 0x11);
  outb(PIC2_COMMAND, 0x11);
  // Potential spurious interrupts at 0x87
  outb(PIC1_DATA, 0x80);  // PIC1 offset (high and dry).
  outb(PIC2_DATA, 0x80);  // PIC2 offset (high and dry).
  outb(PIC1_DATA, 0x4);
  outb(PIC2_DATA, 0x2);
  outb(PIC1_DATA, 0x1);
  outb(PIC2_DATA, 0x1);

  // Mask all.
  outb(PIC1_DATA, 0xFF);
  outb(PIC2_DATA, 0xFF);
}

}  // namespace

Apic* gApic = nullptr;

void Apic::Init() {
  auto config_or = GetApicConfiguration();
  if (!config_or.ok()) {
    panic("Error fetching APIC info from ACPI: {x}", config_or.error());
  }
  gApic = new Apic(config_or.value());
}

void Apic::DumpInfo() {
#if APIC_DEBUG
  dbgln("APIC:");
  dbgln("ID: {x}", GetLocalReg(0x20));
  dbgln("VER: {x}", GetLocalReg(0x30));
  dbgln("TPR: {x}", GetLocalReg(0x80));
  dbgln("APR: {x}", GetLocalReg(0x90));
  dbgln("PPR: {x}", GetLocalReg(0xA0));
  dbgln("LDR: {x}", GetLocalReg(0xD0));
  dbgln("DFR: {x}", GetLocalReg(0xE0));
  dbgln("SIV: {x}", GetLocalReg(0xF0));
  for (uint64_t i = 0; i < 8; i++) {
    dbgln("ISR({}): {x}", i, GetLocalReg(0x100 + (0x10 * i)));
  }
  for (uint64_t i = 0; i < 8; i++) {
    dbgln("TMR({}): {x}", i, GetLocalReg(0x180 + (0x10 * i)));
  }
  for (uint64_t i = 0; i < 8; i++) {
    dbgln("IRR({}): {x}", i, GetLocalReg(0x200 + (0x10 * i)));
  }
  dbgln("ESR: {x}", GetLocalReg(0x280));

  dbgln("IO ID: {x}", GetIoReg(0x0));
  dbgln("IO VER: {x}", GetIoReg(0x1));
  dbgln("IO ARB: {x}", GetIoReg(0x2));
  for (uint8_t i = 0x10; i < 0x3F; i += 2) {
    dbgln("IO ({}): {x}", i, GetIoEntry(i));
  }
  dbgln("APIC MSR: {x}", GetMSR(0x1B));
#endif
}

uint32_t Apic::GetLocalTimerValue() { return GetLocalReg(kTimerCurrOffset); }
void Apic::InitializeLocalTimer(uint32_t init_cnt, TimerMode mode) {
  // FIXME: Don't hardcode this.
  uint32_t vector = 0x21;
  switch (mode) {
    case ONESHOT:
      vector |= LAPIC_TIMER_ONESHOT;
      break;
    case PERIODIC:
      vector |= LAPIC_TIMER_PERIODIC;
      break;
    default:
      panic("Unhandled timer mode.");
  }
  SetLocalReg(kLvtTimerOffset, vector);

  SetLocalReg(kTimerInitOffset, init_cnt);
}

void Apic::SignalEOI() { SetLocalReg(kEoiOffset, 0x0); }

void Apic::UnmaskPit() {
  SetIoDoubleReg(0x14, GetIoDoubleReg(0x14) & ~(APIC_MASK));
}
void Apic::MaskPit() { SetIoDoubleReg(0x14, GetIoDoubleReg(0x14) | APIC_MASK); }

Apic::Apic(const ApicConfiguration& config)
    : l_apic_base_(config.lapic_base),
      io_apic_addr_(
          reinterpret_cast<volatile uint8_t*>(GetPhys(config.ioapic_base))),
      io_apic_data_(GetPhys(config.ioapic_base, 0x10)) {
  MaskPic();

  // Map Timer.
  // FIXME: Get this offset from ACPI.
  SetIoDoubleReg(0x14, 0x20 | APIC_MASK);

  // For now set these based on the presets in the following spec.
  // http://web.archive.org/web/20161130153145/http://download.intel.com/design/chipsets/datashts/29056601.pdf
  // FIXME: However in the future we should likely use the MADT for legacy
  // interrupts and AML for PCI etc.
  // PCI Line 1-4
  // FIXME: These should be level triggered according to spec I believe
  // but because we handle the interrupt outside of the kernel it is tricky
  // to wait to send the end of interrupt message.
  // Because of this leave them as edge triggered and send EOI immediately.
  SetIoDoubleReg(0x30, 0x30);
  SetIoDoubleReg(0x32, 0x31);
  SetIoDoubleReg(0x34, 0x32);
  SetIoDoubleReg(0x36, 0x33);

  DumpInfo();
}

uint32_t Apic::GetLocalReg(uint16_t offset) {
  uint32_t volatile* reg = GetPhys(l_apic_base_, offset);
  return *reg;
}

void Apic::SetLocalReg(uint16_t offset, uint32_t value) {
  *GetPhys(l_apic_base_, offset) = value;
}

uint32_t Apic::GetIoReg(uint8_t offset) {
  *io_apic_addr_ = offset;
  return *io_apic_data_;
}

uint64_t Apic::GetIoDoubleReg(uint8_t offset) {
  *io_apic_addr_ = offset;
  uint64_t entry = *io_apic_data_;
  *io_apic_addr_ = offset + 1;
  entry |= ((uint64_t)*io_apic_data_) << 32;
  return entry;
}

void Apic::SetIoDoubleReg(uint8_t offset, uint64_t value) {
  *io_apic_addr_ = offset;
  *io_apic_data_ = value & 0xFFFFFFFF;
  *io_apic_addr_ = offset + 1;
  *io_apic_data_ = value >> 32;
}
