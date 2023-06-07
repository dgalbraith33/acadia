#include <mammoth/debug.h>
#include <mammoth/process.h>
#include <zcall.h>

int main() {
  dbgln("Testing");
  uint64_t vaddr;
  check(ZAddressSpaceMap(Z_INIT_AS_SELF, 0, Z_INIT_BOOT_VMMO, &vaddr));
  check(SpawnProcessFromElfRegion(vaddr));
  dbgln("Return");
  return 0;
}
