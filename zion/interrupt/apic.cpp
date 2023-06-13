#include "interrupt/apic.h"

#include <stdint.h>

#include "boot/boot_info.h"
#include "common/msr.h"
#include "common/port.h"
#include "debug/debug.h"

#define APIC_DEBUG 1

namespace {

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100  // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800

const uint64_t kEoiOffset = 0xB0;

// FIXME: parse these from madt.
constexpr uint64_t kLApicBase = 0xFEE0'0000;
constexpr uint64_t kIoApicAddr = 0xFEC0'0000;
constexpr uint64_t kIoApicData = 0xFEC0'0010;

uint32_t volatile* GetPhys(uint64_t base, uint64_t offset = 0) {
  return reinterpret_cast<uint32_t*>(boot::GetHigherHalfDirectMap() + base +
                                     offset);
}

uint32_t GetLocalReg(uint64_t offset) {
  uint32_t volatile* reg = GetPhys(kLApicBase, offset);
  return *reg;
}

void WriteLocalReg(uint64_t offset, uint32_t value) {
  *GetPhys(kLApicBase, offset) = value;
}

uint32_t GetIoReg(uint8_t reg) {
  *GetPhys(kIoApicAddr) = reg;
  return *GetPhys(kIoApicData);
}

uint64_t GetIoEntry(uint8_t reg) {
  *GetPhys(kIoApicAddr) = reg;
  uint64_t entry = *GetPhys(kIoApicData);
  *GetPhys(kIoApicAddr) = reg + 1;
  entry |= ((uint64_t)*GetPhys(kIoApicData)) << 32;
  return entry;
}

void SetIoEntry(uint8_t reg, uint64_t value) {
  *GetPhys(kIoApicAddr) = reg;
  *GetPhys(kIoApicData) = value & 0xFFFFFFFF;
  *GetPhys(kIoApicAddr) = reg + 1;
  *GetPhys(kIoApicData) = value >> 32;
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

void InspectApic() {
#if APIC_DEBUG
  dbgln("APIC:");
  dbgln("ID: %x", GetLocalReg(0x20));
  dbgln("VER: %x", GetLocalReg(0x30));
  dbgln("TPR: %x", GetLocalReg(0x80));
  dbgln("APR: %x", GetLocalReg(0x90));
  dbgln("PPR: %x", GetLocalReg(0xA0));
  dbgln("LDR: %x", GetLocalReg(0xD0));
  dbgln("DFR: %x", GetLocalReg(0xE0));
  dbgln("SIV: %x", GetLocalReg(0xF0));
  for (uint64_t i = 0; i < 8; i++) {
    dbgln("ISR(%u): %x", i, GetLocalReg(0x100 + (0x10 * i)));
  }
  for (uint64_t i = 0; i < 8; i++) {
    dbgln("TMR(%u): %x", i, GetLocalReg(0x180 + (0x10 * i)));
  }
  for (uint64_t i = 0; i < 8; i++) {
    dbgln("IRR(%u): %x", i, GetLocalReg(0x200 + (0x10 * i)));
  }
  dbgln("ESR: %x", GetLocalReg(0x280));

  dbgln("IO ID: %x", GetIoReg(0x0));
  dbgln("IO VER: %x", GetIoReg(0x1));
  dbgln("IO ARB: %x", GetIoReg(0x2));
  for (uint8_t i = 0x10; i < 0x3F; i += 2) {
    dbgln("IO (%u): %x", i, GetIoEntry(i));
  }
  dbgln("APIC MSR: %x", GetMSR(0x1B));
#endif
}

void EnableApic() {
  MaskPic();
  // Map Timer.
  SetIoEntry(0x14, 0x20);
  // Skip Keyboard for now.
  // SetIoEntry(0x12, 0x21);

  // TODO: This also works with the interrupt numbers provided by the MADT
  // I need to do further investigation on the difference in this case and
  // also how to find a declarative spec for where the PCI Lines are mapped.

  // PCI Line 1-4
  // FIXME: These should be level triggered according to spec I believe
  // but because we handle the interrupt outside of the kernel it is tricky
  // to wait to send the end of interrupt message.
  // Because of this leave them as edge triggered and send EOI immediately.
  SetIoEntry(0x30, 0x30);
  SetIoEntry(0x32, 0x31);
  SetIoEntry(0x34, 0x32);
  SetIoEntry(0x36, 0x33);

  InspectApic();
}

void SignalEOI() { WriteLocalReg(kEoiOffset, 0x0); }
