#include <glacier/string/str_format.h>
#include <glacier/string/str_split.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/memory_region.h>
#include <mammoth/process.h>
#include <zcall.h>
#include <ztypes.h>

#include "hw/gpt.h"
#include "hw/pcie.h"
#include "yellowstone_server.h"

glcr::ErrorCode SpawnProcess(z_cap_t vmmo_cap, z_cap_t yellowstone_cap) {
  OwnedMemoryRegion region = OwnedMemoryRegion::FromCapability(vmmo_cap);
  return SpawnProcessFromElfRegion(region.vaddr(), yellowstone_cap);
}

uint64_t main(uint64_t port_cap) {
  check(ParseInitPort(port_cap));
  dbgln("Yellowstone Initializing.");

  ASSIGN_OR_RETURN(auto server, YellowstoneServer::Create());
  Thread server_thread = server->RunServer();

  ASSIGN_OR_RETURN(YellowstoneClient client1, server->CreateClient());
  check(SpawnProcess(gBootDenaliVmmoCap, client1.Capability()));

  server->WaitDenaliRegistered();

  ASSIGN_OR_RETURN(YellowstoneClient client2, server->CreateClient());
  check(SpawnProcess(gBootVictoriaFallsVmmoCap, client2.Capability()));

  server->WaitVictoriaFallsRegistered();

  dbgln("VFS Available.");

  auto vfs_client = server->GetVFSClient();
  OpenFileRequest request;
  request.set_path("/init.txt");
  OpenFileResponse response;
  check(vfs_client->OpenFile(request, response));

  OwnedMemoryRegion filemem =
      OwnedMemoryRegion::FromCapability(response.memory());
  glcr::String file(reinterpret_cast<const char*>(filemem.vaddr()),
                    response.size());

  glcr::Vector<glcr::StringView> files = glcr::StrSplit(file, '\n');

  for (uint64_t i = 0; i < files.size(); i++) {
    if (!files[i].empty()) {
      dbgln("Starting {}", files[i]);
      OpenFileRequest req;
      req.set_path(glcr::StrFormat("/bin/{}", files[i]));
      OpenFileResponse resp;
      check(vfs_client->OpenFile(req, resp));

      ASSIGN_OR_RETURN(YellowstoneClient client3, server->CreateClient());
      check(SpawnProcess(resp.memory(), client3.Capability()));
    }
  }

  check(server_thread.Join());
  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
