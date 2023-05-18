#include <stdint.h>

#define COM1 0x3f8

void outb(uint16_t port, uint8_t value) {
  asm volatile("outb %0, %1" ::"a"(value), "Nd"(port));
}

extern "C" void zion() {
  outb(COM1, 'a');

  while (1)
    ;
}
