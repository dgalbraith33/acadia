
#include <mammoth/debug.h>
#include <mammoth/endpoint_server.h>
#include <mammoth/init.h>
#include <mammoth/port_client.h>
#include <stdint.h>
#include <yellowstone.h>
#include <yellowstone_stub.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

uint64_t main(uint64_t init_port_cap) {
  check(ParseInitPort(init_port_cap));
  glcr::UniquePtr<EndpointClient> yellowstone =
      EndpointClient::AdoptEndpoint(gInitEndpointCap);

  YellowstoneStub stub(gInitEndpointCap);
  ASSIGN_OR_RETURN(MappedMemoryRegion ahci_region, stub.GetAhciConfig());
  ASSIGN_OR_RETURN(auto driver, AhciDriver::Init(ahci_region));

  ASSIGN_OR_RETURN(glcr::UniquePtr<DenaliServer> server,
                   DenaliServer::Create(*driver));

  ASSIGN_OR_RETURN(glcr::UniquePtr<EndpointClient> client,
                   server->CreateClient());
  check(stub.Register("denali", *client));

  Thread server_thread = server->RunServer();

  check(server_thread.Join());
  return 0;
}
