#include "interrupt/interrupt.h"

#include <stdint.h>

#include "common/port.h"
#include "debug/debug.h"
#include "interrupt/apic.h"
#include "interrupt/apic_timer.h"
#include "memory/kernel_heap.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"

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

InterruptDescriptor CreateDescriptor(void isr(void), uint8_t ist = 0) {
  uint64_t offset = reinterpret_cast<uint64_t>(isr);
  return InterruptDescriptor{
      .offset_low = static_cast<uint16_t>(offset),
      .selector = KERNEL_CS,
      .ist = ist,
      .flags = IDT_INTERRUPT_GATE,
      .offset_medium = static_cast<uint16_t>(offset >> 16),
      .offset_high = static_cast<uint32_t>(offset >> 32),
      .zero = 0x0,
  };
}

struct InterruptFrame {
  uint64_t cr2;
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
extern "C" void interrupt_divide_by_zero(InterruptFrame* frame) {
  dbgln("RIP: {x}", frame->rip);
  panic("DIV0");
}

extern "C" void isr_invalid_opcode();
extern "C" void interrupt_invalid_opcode(InterruptFrame* frame) {
  dbgln("RIP: {x}", frame->rip);
  panic("INVALID OPCODE");
}

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
  dbgln("Index: {}", err >> 3);
  dbgln("RIP: {x}", frame->rip);
  dbgln("RSP: {x}", frame->rsp);

  panic("GP");
}

extern "C" void isr_page_fault();
extern "C" void interrupt_page_fault(InterruptFrame* frame) {
  uint64_t err = frame->error_code;
  if ((err & 0x1) == 0) {
    // Page not present error may be resolveable.
    if (gScheduler &&
        gScheduler->CurrentProcess().vmas()->HandlePageFault(frame->cr2)) {
      return;
    }
  }
  dbgln("Unhandled Page Fault:");
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

  if (err & 0x4) {
    dbgln("User Space");
  }

  if (err & 0x10) {
    dbgln("Instruction Fetch");
  }

  dbgln("rip:  {x}", frame->rip);
  dbgln("addr: {x}", frame->cr2);
  panic("PF");
}

extern "C" void isr_fpe_fault();
extern "C" void interrupt_fpe_fault(InterruptFrame* frame) {
  panic("Floating point exception");
}

extern "C" void isr_timer();
extern "C" void interrupt_timer(InterruptFrame*) {
  gApicTimer->Calibrate();
  gApic->SignalEOI();
}

uint64_t cnt = 0;
extern "C" void isr_apic_timer();
extern "C" void interrupt_apic_timer(InterruptFrame*) {
  cnt++;
  if (cnt % 20 == 0) {
    if (cnt == 20) {
      KernelHeap::DumpDebugData();
      phys_mem::DumpPhysicalMemoryUsage();
    }
    dbgln("timer: {}s", cnt * 50 / 1000);
  }
  gApic->SignalEOI();
  gScheduler->Preempt();
}

glcr::RefPtr<Port> pci1_port;
extern "C" void isr_pci1();
extern "C" void interrupt_pci1(InterruptFrame*) {
  pci1_port->Send({});
  gApic->SignalEOI();
}

extern "C" void isr_pci2();
extern "C" void interrupt_pci2(InterruptFrame*) {
  dbgln("Interrupt PCI line 2");
  gApic->SignalEOI();
}

extern "C" void isr_pci3();
extern "C" void interrupt_pci3(InterruptFrame*) {
  dbgln("Interrupt PCI line 3");
  gApic->SignalEOI();
}

extern "C" void isr_pci4();
extern "C" void interrupt_pci4(InterruptFrame*) {
  dbgln("Interrupt PCI line 4");
  gApic->SignalEOI();
}

void InitIdt() {
  gIdt[0] = CreateDescriptor(isr_divide_by_zero);
  gIdt[0x6] = CreateDescriptor(isr_invalid_opcode);
  gIdt[0xD] = CreateDescriptor(isr_protection_fault);
  gIdt[0xE] = CreateDescriptor(isr_page_fault);
  gIdt[0x13] = CreateDescriptor(isr_fpe_fault);

  gIdt[0x20] = CreateDescriptor(isr_timer);
  gIdt[0x21] = CreateDescriptor(isr_apic_timer);

  gIdt[0x30] = CreateDescriptor(isr_pci1);
  gIdt[0x31] = CreateDescriptor(isr_pci2);
  gIdt[0x32] = CreateDescriptor(isr_pci3);
  gIdt[0x33] = CreateDescriptor(isr_pci4);

  InterruptDescriptorTablePointer idtp{
      .size = sizeof(gIdt),
      .base = reinterpret_cast<uint64_t>(gIdt),
  };
  asm volatile("lidt %0" ::"m"(idtp));
}

void UpdateFaultHandlersToIst1() {
  gIdt[13] = CreateDescriptor(isr_protection_fault, 1);
  gIdt[14] = CreateDescriptor(isr_page_fault, 1);

  InterruptDescriptorTablePointer idtp{
      .size = sizeof(gIdt),
      .base = reinterpret_cast<uint64_t>(gIdt),
  };
  asm volatile("lidt %0" ::"m"(idtp));
}

void RegisterPciPort(const glcr::RefPtr<Port>& port) { pci1_port = port; }
