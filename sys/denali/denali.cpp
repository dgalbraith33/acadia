
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <stdint.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

uint64_t main(uint64_t init_port_cap) {
  check(ParseInitPort(init_port_cap));

  YellowstoneClient stub(gInitEndpointCap);
  AhciInfo ahci;
  RET_ERR(stub.GetAhciInfo(ahci));
  mmth::OwnedMemoryRegion ahci_region =
      mmth::OwnedMemoryRegion::FromCapability(ahci.ahci_region());
  ASSIGN_OR_RETURN(auto driver, AhciDriver::Init(glcr::Move(ahci_region)));

  ASSIGN_OR_RETURN(glcr::UniquePtr<DenaliServer> server,
                   DenaliServer::Create(*driver));

  Thread server_thread = server->RunServer();

  RegisterEndpointRequest req;
  req.set_endpoint_name("denali");
  ASSIGN_OR_RETURN(z_cap_t client_cap, server->CreateClientCap());
  req.set_endpoint_capability(client_cap);
  check(stub.RegisterEndpoint(req));

  check(server_thread.Join());
  return 0;
}
