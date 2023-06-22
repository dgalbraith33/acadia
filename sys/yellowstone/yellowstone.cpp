#include <denali/denali.h>
#include <mammoth/debug.h>
#include <mammoth/endpoint_client.h>
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

  auto endpoint_or = SpawnProcessFromElfRegion(vaddr);
  if (!endpoint_or) {
    check(endpoint_or.error());
  }
  EndpointClient endpoint = endpoint_or.value();

  DenaliClient client(endpoint);
  GptReader reader(client);

  check(reader.ParsePartitionTables());

  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootVictoriaFallsVmmoCap, &vaddr));
  auto error_or = SpawnProcessFromElfRegion(vaddr);
  if (!error_or) {
    check(error_or.error());
  }
  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
