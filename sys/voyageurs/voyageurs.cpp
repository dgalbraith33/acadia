
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "keyboard/keyboard_driver.h"
#include "voyageurs_server.h"
#include "xhci/xhci_driver.h"

using yellowstone::RegisterEndpointRequest;
using yellowstone::YellowstoneClient;

uint64_t main(uint64_t init_port) {
  ParseInitPort(init_port);

  YellowstoneClient yellowstone(gInitEndpointCap);

  ASSIGN_OR_RETURN(auto xhci, XhciDriver::InitiateDriver(yellowstone));

  dbgln("Initializing PS/2 Driver.");
  KeyboardDriver driver;

  dbgln("Starting PS/2 Thread.");
  Thread keyboard_thread = driver.StartInterruptLoop();

  dbgln("Starting voyaguers server.");
  ASSIGN_OR_RETURN(glcr::UniquePtr<VoyageursServer> server,
                   VoyageursServer::Create(driver));

  Thread server_thread = server->RunServer();

  RegisterEndpointRequest req;
  req.set_endpoint_name("voyageurs");
  ASSIGN_OR_RETURN(z_cap_t client_cap, server->CreateClientCap());
  req.set_endpoint_capability(client_cap);
  check(yellowstone.RegisterEndpoint(req));

  check(server_thread.Join());
  check(keyboard_thread.Join());

  return glcr::OK;
}
