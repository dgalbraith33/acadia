#include <denali/denali.h>
#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/pcie.h"

uint64_t main() {
  dbgln("Yellowstone Initializing.");
  uint64_t vaddr;
  check(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, Z_INIT_BOOT_VMMO, &vaddr));

  Channel local;
  check(SpawnProcessFromElfRegion(vaddr, local));
  local.WriteStr("Hello!");

  DumpPciEDevices();

  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
