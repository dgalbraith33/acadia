
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
  AhciDriver driver;
  RET_ERR(driver.Init());

  EndpointClient yellowstone = EndpointClient::AdoptEndpoint(gInitEndpointCap);
  YellowstoneGetReq req{
      .type = kYellowstoneGetRegistration,
  };
  auto resp_cap_or =
      yellowstone
          .CallEndpoint<YellowstoneGetReq, YellowstoneGetRegistrationResp>(req);
  if (!resp_cap_or.ok()) {
    dbgln("Bad call");
    check(resp_cap_or.error());
  }
  auto resp_cap = resp_cap_or.value();
  PortClient notify = PortClient::AdoptPort(resp_cap.second());

  ASSIGN_OR_RETURN(EndpointServer endpoint, EndpointServer::Create());
  ASSIGN_OR_RETURN(EndpointClient client, endpoint.CreateClient());
  notify.WriteMessage("denali", client.GetCap());

  DenaliServer server(endpoint, driver);
  RET_ERR(server.RunServer());
  // FIXME: Add thread join.
  return 0;
}
