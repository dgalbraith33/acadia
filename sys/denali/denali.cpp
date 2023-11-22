
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <stdint.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

uint64_t main(uint64_t init_port_cap) {
  check(ParseInitPort(init_port_cap));

  YellowstoneClient stub(gInitEndpointCap);
  Empty empty;
  AhciInfo ahci;
  RET_ERR(stub.GetAhciInfo(empty, ahci));
  OwnedMemoryRegion ahci_region =
      OwnedMemoryRegion::FromCapability(ahci.ahci_region());
  ASSIGN_OR_RETURN(auto driver, AhciDriver::Init(glcr::Move(ahci_region)));

  ASSIGN_OR_RETURN(glcr::UniquePtr<DenaliServer> server,
                   DenaliServer::Create(*driver));

  ASSIGN_OR_RETURN(DenaliClient client, server->CreateClient());

  Thread server_thread = server->RunServer();

  RegisterEndpointRequest req;
  req.set_endpoint_name("denali");
  req.set_endpoint_capability(client.Capability());
  check(stub.RegisterEndpoint(req, empty));

  check(server_thread.Join());
  return 0;
}
