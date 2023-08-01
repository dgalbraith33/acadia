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
  Thread registration_thread = server->RunRegistration();

  uint64_t vaddr;
  check(ZAddressSpaceMap(gSelfVmasCap, 0, gBootDenaliVmmoCap, &vaddr));
  ASSIGN_OR_RETURN(glcr::UniquePtr<EndpointClient> client,
                   server->CreateClient());
  check(SpawnProcessFromElfRegion(vaddr, glcr::Move(client)));

  check(server->RunServer());
  check(registration_thread.Join());
  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
