#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "fs/ext2/ext2_driver.h"
#include "victoriafalls_server.h"

using yellowstone::DenaliInfo;
using yellowstone::RegisterEndpointRequest;
using yellowstone::YellowstoneClient;

uint64_t main(uint64_t init_cap) {
  ParseInitPort(init_cap);

  dbgln("VFs Started");

  YellowstoneClient yellowstone(gInitEndpointCap);
  DenaliInfo denali_info;
  check(yellowstone.GetDenali(denali_info));
  ASSIGN_OR_RETURN(Ext2Driver ext2, Ext2Driver::Init(denali_info));

  ASSIGN_OR_RETURN(auto server, VFSServer::Create(ext2));

  Thread server_thread = server->RunServer();

  RegisterEndpointRequest req;
  req.set_endpoint_name("victoriafalls");
  ASSIGN_OR_RETURN(auto client_cap, server->CreateClientCap());
  req.set_endpoint_capability(client_cap);
  check(yellowstone.RegisterEndpoint(req));

  RET_ERR(server_thread.Join());

  return 0;
}
