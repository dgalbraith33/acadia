
#include <mammoth/debug.h>
#include <mammoth/endpoint_server.h>
#include <mammoth/init.h>
#include <mammoth/port_client.h>
#include <stdint.h>
#include <yellowstone.h>

#include "ahci/ahci_driver.h"
#include "denali_server.h"

uint64_t main(uint64_t init_port_cap) {
  check(ParseInitPort(init_port_cap));
  glcr::UniquePtr<EndpointClient> yellowstone =
      EndpointClient::AdoptEndpoint(gInitEndpointCap);
  YellowstoneGetReq ahci_req{
      .type = kYellowstoneGetAhci,
  };
  auto resp_or =
      yellowstone->CallEndpoint<YellowstoneGetReq, YellowstoneGetAhciResp>(
          ahci_req);
  if (!resp_or.ok()) {
    check(resp_or.error());
  }

  uint64_t ahci_addr = resp_or.value().ahci_phys_offset;
  AhciDriver driver(ahci_addr);
  RET_ERR(driver.Init());

  YellowstoneGetReq req{
      .type = kYellowstoneGetRegistration,
  };
  auto resp_cap_or =
      yellowstone->CallEndpointGetCap<YellowstoneGetReq,
                                      YellowstoneGetRegistrationResp>(req);
  if (!resp_cap_or.ok()) {
    dbgln("Bad call");
    check(resp_cap_or.error());
  }
  auto resp_cap = resp_cap_or.value();
  PortClient notify = PortClient::AdoptPort(resp_cap.second());

  ASSIGN_OR_RETURN(glcr::UniquePtr<EndpointServer> endpoint,
                   EndpointServer::Create());
  ASSIGN_OR_RETURN(glcr::UniquePtr<EndpointClient> client,
                   endpoint->CreateClient());
  notify.WriteMessage("denali", client->GetCap());

  DenaliServer server(glcr::Move(endpoint), driver);
  RET_ERR(server.RunServer());
  // FIXME: Add thread join.
  return 0;
}
