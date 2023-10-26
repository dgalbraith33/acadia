#include <mammoth/debug.h>
#include <mammoth/endpoint_client.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <zcall.h>

#include "hw/gpt.h"
#include "hw/pcie.h"
#include "yellowstone_server.h"

glcr::ErrorCode SpawnProcess(z_cap_t vmmo_cap, z_cap_t yellowstone_cap) {
  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(gSelfVmasCap, 0, vmmo_cap, &vaddr));
  return SpawnProcessFromElfRegion(vaddr, yellowstone_cap);
}

uint64_t main(uint64_t port_cap) {
  dbgln("Yellowstone Initializing.");
  check(ParseInitPort(port_cap));

  ASSIGN_OR_RETURN(auto server, YellowstoneServer::Create());
  Thread server_thread = server->RunServer();

  ASSIGN_OR_RETURN(YellowstoneClient client1, server->CreateClient());
  check(SpawnProcess(gBootDenaliVmmoCap, client1.Capability()));

  check(server->WaitDenaliRegistered());

  ASSIGN_OR_RETURN(YellowstoneClient client2, server->CreateClient());
  check(SpawnProcess(gBootVictoriaFallsVmmoCap, client2.Capability()));

  check(server_thread.Join());
  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
