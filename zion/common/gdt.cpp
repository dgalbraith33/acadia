#include "common/gdt.h"

#include <stdint.h>

#define GDT_ACCESSED 1
#define GDT_READ_WRITE (1 << 1)
#define GDT_EXECUTABLE (1 << 3)
#define GDT_SEGMENT (1 << 4)
#define GDT_RING3 (3 << 5)
#define GDT_PRESENT (1 << 7)

#define GDT_FLAGS (1 << 7) | (1 << 5)  // 4K granularity | long mode

struct TaskStateSegment {
  uint32_t reserved = 0;
  uint64_t rsp0 = 0;
  uint64_t rsp1 = 0;
  uint64_t rsp2 = 0;
  uint64_t reserved2 = 0;
  uint64_t ist1 = 0;
  uint64_t ist2 = 0;
  uint64_t ist3 = 0;
  uint64_t ist4 = 0;
  uint64_t ist5 = 0;
  uint64_t ist6 = 0;
  uint64_t ist7 = 0;
  uint64_t reserved3 = 0;
  uint16_t reserved4 = 0;
  uint16_t iomap_base = 0;
} __attribute__((packed));

struct GdtPointer {
  uint16_t size;
  uint64_t base;
} __attribute__((packed));

static uint64_t gGdtSegments[7];
static TaskStateSegment gTaskStateSegment;

// Defined in load_gdt.s
extern "C" void ReloadSegments();

uint64_t CreateSegment(uint64_t access, uint64_t flags) {
  uint64_t base = 0;
  access &= 0xFF;
  flags &= 0xF0;
  flags |= 0xF;  // For the highest 4 bits of the limit.
  // Lowest bits are the limit (always set to max);
  return (0xFFFF) | (access << 40) | (flags << 48);
}

uint64_t CreateTssSegment(TaskStateSegment* tss) {
  uint64_t base = reinterpret_cast<uint64_t>(tss);
  uint64_t limit = sizeof(TaskStateSegment);
  uint64_t access = GDT_ACCESSED | GDT_PRESENT | GDT_EXECUTABLE;
  return limit | ((base & 0xFFFF) << 16) | ((base >> 16 & 0xFF) << 32) |
         (access << 40) | (((base >> 24) & 0xFF) << 56);
}

void InitGdt() {
  gGdtSegments[0] = CreateSegment(0, 0);
  uint64_t default_bits = GDT_PRESENT | GDT_SEGMENT | GDT_READ_WRITE;

  // Kernel CS
  gGdtSegments[1] = CreateSegment(default_bits | GDT_EXECUTABLE, GDT_FLAGS);

  // Kernel DS
  gGdtSegments[2] = CreateSegment(default_bits, GDT_FLAGS);

  // User CS
  gGdtSegments[3] =
      CreateSegment(default_bits | GDT_RING3 | GDT_EXECUTABLE, GDT_FLAGS);

  // User DS
  gGdtSegments[4] = CreateSegment(default_bits | GDT_RING3, GDT_FLAGS);

  gTaskStateSegment.iomap_base = sizeof(TaskStateSegment);
  gGdtSegments[5] = CreateTssSegment(&gTaskStateSegment);
  gGdtSegments[6] = reinterpret_cast<uint64_t>(&gTaskStateSegment) >> 32;

  GdtPointer gdtp{
      .size = sizeof(gGdtSegments) - 1,
      .base = reinterpret_cast<uint64_t>(gGdtSegments),
  };

  asm volatile("lgdt %0" ::"m"(gdtp));
  ReloadSegments();
  asm volatile(
      "mov $0x28, %%ax;"
      "ltr %%ax;" ::
          : "rax");
}
