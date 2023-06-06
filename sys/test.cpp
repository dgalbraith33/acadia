
#include "zcall.h"
#include "zerrors.h"

constexpr uint64_t prog2 = 0x00000020'00000000;

int main() {
  ZDebug("Testing");
  uint64_t err = ZProcessSpawnElf(Z_INIT_PROC_SELF, prog2, 0x2000);
  if (err != Z_OK) {
    ZDebug("Error");
  } else {
    ZDebug("Return");
  }
  return 0;
}
