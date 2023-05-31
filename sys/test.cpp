
#include "zcall.h"

constexpr uint64_t prog2 = 0x00000020'00000000;

int main() {
  ZDebug("Testing");
  ZProcessSpawn(prog2, 0x1000);
  ZDebug("Return");
  return 0;
}
