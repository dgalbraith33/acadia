#include <denali/denali.h>
#include <mammoth/channel.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/gpt.h"
#include "hw/pcie.h"

uint64_t main(uint64_t port_cap) {
  dbgln("Yellowstone Initializing.");
  check(ParseInitPort(port_cap));

  DumpPciEDevices();

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootDenaliVmmoCap, &vaddr));

  Channel local;
  check(SpawnProcessFromElfRegion(vaddr, local));

  DenaliClient client(local);
  GptReader reader(client);

  check(reader.ParsePartitionTables());

  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
