#include "interrupt/interrupt.h"

#include <stdint.h>

#include "common/port.h"
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

struct InterruptFrame {
  uint64_t rax;
  uint64_t rbx;
  uint64_t rcx;
  uint64_t rdx;
  uint64_t rsi;
  uint64_t rdi;
  uint64_t r8;
  uint64_t r9;
  uint64_t r10;
  uint64_t r11;
  uint64_t r12;
  uint64_t r13;
  uint64_t r14;
  uint64_t r15;
  uint64_t rbp;
  uint64_t error_code;
  uint64_t rip;
  uint64_t cs;
  uint64_t eflags;
  uint64_t rsp;
  uint64_t ss;
};

extern "C" void isr_divide_by_zero();
extern "C" void interrupt_divide_by_zero(void* frame) { panic("DIV0"); }

extern "C" void isr_protection_fault();
extern "C" void interrupt_protection_fault(InterruptFrame* frame) {
  dbgln("General Protection Fault");
  uint64_t err = frame->error_code;
  if (err & 0x1) {
    dbgln("External Source");
  }
  if (err & 0x2) {
    dbgln("IDT");
  } else {
    dbgln("GDT");
  }
  dbgln("Index: %u", err >> 3);

  panic("GP");
}

extern "C" void isr_page_fault();
extern "C" void interrupt_page_fault(InterruptFrame* frame) {
  dbgln("Page Fault:");
  uint64_t err = frame->error_code;
  if (err & 0x1) {
    dbgln("Page Protection");
  } else {
    dbgln("Page Not Present");
  }

  if (err & 0x2) {
    dbgln("Write");
  } else {
    dbgln("Read");
  }

  if (err & 0x8) {
    dbgln("Instruction Fetch");
  }

  uint64_t cr2;
  asm volatile("mov %%cr2, %0" : "=r"(cr2));
  dbgln("rip:  %m", frame->rip);
  dbgln("addr: %m", cr2);
  panic("PF");
}

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC_EOI 0x20

uint64_t cnt = 0;
extern "C" void isr_timer();
extern "C" void interrupt_timer(InterruptFrame*) {
  cnt++;
  if (cnt % 20 == 0) {
    dbgln("timer: %us", cnt * 50 / 1000);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}

void EnablePic() {
  outb(PIC1_COMMAND, 0x11);
  outb(PIC1_DATA, 0x20);  // PIC1 offset.
  outb(PIC1_DATA, 0x4);
  outb(PIC1_DATA, 0x1);

  // Mask all except the timer.
  outb(PIC1_DATA, 0xE);
}

void InitIdt() {
  gIdt[0] = CreateDescriptor(isr_divide_by_zero);
  gIdt[13] = CreateDescriptor(isr_protection_fault);
  gIdt[14] = CreateDescriptor(isr_page_fault);
  gIdt[32] = CreateDescriptor(isr_timer);
  InterruptDescriptorTablePointer idtp{
      .size = sizeof(gIdt),
      .base = reinterpret_cast<uint64_t>(gIdt),
  };
  asm volatile("lidt %0" ::"m"(idtp));

  EnablePic();
}
