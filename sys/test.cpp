
#include "zcall.h"
#include "zerrors.h"

constexpr uint64_t prog2 = 0x00000020'00000000;

int main() {
  ZDebug("Testing");
  uint64_t err = ZProcessSpawnElf(0x100, prog2, 0x1000);
  if (err != Z_OK) {
    ZDebug("Error");
  } else {
    ZDebug("Return");
  }
  return 0;
}
