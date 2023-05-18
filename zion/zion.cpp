#include <stdint.h>

#include "debug/debug.h"

extern "C" void zion() {
  dbgln("Hello World!");

  while (1)
    ;
}
