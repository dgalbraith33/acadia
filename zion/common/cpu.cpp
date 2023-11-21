#include "common/cpu.h"

#include <cpuid.h>

#include "debug/debug.h"

void ProbeCpuAndEnableFeatures() {
  dbgln("CPUID");
  uint32_t eax, ebx, ecx, edx;
  __get_cpuid(1, &eax, &ebx, &ecx, &edx);

  if (!(edx & (0x3 << 25))) {
    panic("SSE & SSE2 not available.");
  }

  if (!(ecx & (0x1 | (0x1 << 9)))) {
    panic("SSE3, SSSE3 not available.");
  }

  if (!(ecx & (0x3 << 19))) {
    dbgln("SSE4 not available.");
  }

  dbgln("Setting SSE");
  asm volatile(
      "mov %%cr0, %%rax;"
      "and $0xFFFB, %%ax;"  // Clear EM
      "or $0x2, %%ax;"      // Set MP
      "mov %%rax, %%cr0;"
      "mov %%cr4, %%rax;"
      "or $0x600, %%ax;"  // Set OSFXSR, OSXMMEXCPT
      "mov %%rax, %%cr4;" ::
          : "rax");
}
