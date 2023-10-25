
#include <mammoth/debug.h>
#include <mammoth/endpoint_server.h>
#include <mammoth/init.h>
#include <mammoth/port_client.h>
#include <stdint.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

uint64_t main(uint64_t init_port_cap) {
  check(ParseInitPort(init_port_cap));
  glcr::UniquePtr<EndpointClient> yellowstone =
      EndpointClient::AdoptEndpoint(gInitEndpointCap);

  YellowstoneClient stub(gInitEndpointCap);
  Empty empty;
  AhciInfo ahci;
  RET_ERR(stub.GetAhciInfo(empty, ahci));
  MappedMemoryRegion ahci_region =
      MappedMemoryRegion::FromCapability(ahci.ahci_region());
  ASSIGN_OR_RETURN(auto driver, AhciDriver::Init(ahci_region));

  ASSIGN_OR_RETURN(glcr::UniquePtr<DenaliServer> server,
                   DenaliServer::Create(*driver));

  ASSIGN_OR_RETURN(glcr::UniquePtr<EndpointClient> client,
                   server->CreateClient());

  Thread server_thread = server->RunServer();

  RegisterEndpointRequest req;
  req.set_endpoint_name("denali");
  req.set_endpoint_capability(client->GetCap());
  check(stub.RegisterEndpoint(req, empty));

  check(server_thread.Join());
  return 0;
}
