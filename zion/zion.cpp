#include <stdint.h>

#include "common/port.h"

#define COM1 0x3f8

extern "C" void zion() {
  outb(COM1, 'a');

  while (1)
    ;
}
