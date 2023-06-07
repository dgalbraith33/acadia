#include <mammoth/debug.h>
#include <mammoth/process.h>

constexpr uint64_t prog2 = 0x00000020'00000000;

int main() {
  dbgln("Testing");
  check(SpawnProcessFromElfRegion(prog2));
  dbgln("Return");
  return 0;
}
