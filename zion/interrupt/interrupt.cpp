#include "interrupt/interrupt.h"

#include <stdint.h>

#include "debug/debug.h"

#define IDT_INTERRUPT_GATE 0x8E

#define KERNEL_CS 0x8

struct InterruptDescriptorTablePointer {
  uint16_t size;
  uint64_t base;
} __attribute__((packed));

struct InterruptDescriptor {
  uint16_t offset_low;
  uint16_t selector;
  uint8_t ist;
  uint8_t flags;
  uint16_t offset_medium;
  uint32_t offset_high;
  uint32_t zero;
} __attribute__((packed));

static InterruptDescriptor gIdt[256];

InterruptDescriptor CreateDescriptor(void isr(void)) {
  uint64_t offset = reinterpret_cast<uint64_t>(isr);
  return InterruptDescriptor{
      .offset_low = static_cast<uint16_t>(offset),
      .selector = KERNEL_CS,
      .ist = 0,
      .flags = IDT_INTERRUPT_GATE,
      .offset_medium = static_cast<uint16_t>(offset >> 16),
      .offset_high = static_cast<uint32_t>(offset >> 32),
      .zero = 0x0,
  };
}

extern "C" void isr_divide_by_zero();
extern "C" void interrupt_divide_by_zero(void* frame) { panic("DIV0"); }

extern "C" void isr_protection_fault();
extern "C" void interrupt_protection_fault(void* frame) { panic("GP"); }

extern "C" void isr_page_fault();
extern "C" void interrupt_page_fault(void* frame) { panic("PF"); }

void InitIdt() {
  gIdt[0] = CreateDescriptor(isr_divide_by_zero);
  gIdt[13] = CreateDescriptor(isr_protection_fault);
  gIdt[14] = CreateDescriptor(isr_page_fault);
  InterruptDescriptorTablePointer idtp{
      .size = sizeof(gIdt),
      .base = reinterpret_cast<uint64_t>(gIdt),
  };
  asm volatile("lidt %0" ::"m"(idtp));
}
