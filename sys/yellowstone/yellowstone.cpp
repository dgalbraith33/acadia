#include <mammoth/debug.h>
#include <mammoth/endpoint_client.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/gpt.h"
#include "hw/pcie.h"
#include "yellowstone_server.h"

uint64_t main(uint64_t port_cap) {
  dbgln("Yellowstone Initializing.");
  check(ParseInitPort(port_cap));

  ASSIGN_OR_RETURN(auto server, YellowstoneServer::Create());
  Thread server_thread = server->RunServer();
  Thread registration_thread = server->RunRegistration();
  DumpPciEDevices();

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootDenaliVmmoCap, &vaddr));
  ASSIGN_OR_RETURN(EndpointClient client, server->GetServerClient());
  check(SpawnProcessFromElfRegion(vaddr, client));

  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootVictoriaFallsVmmoCap, &vaddr));
  ASSIGN_OR_RETURN(client, server->GetServerClient());
  check(SpawnProcessFromElfRegion(vaddr, client));

  check(server_thread.Join());
  check(registration_thread.Join());
  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
