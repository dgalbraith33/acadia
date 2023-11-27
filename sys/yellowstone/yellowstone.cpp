#include <glacier/string/str_format.h>
#include <glacier/string/str_split.h>
#include <mammoth/file/file.h>
#include <mammoth/proc/process.h>
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <mammoth/util/memory_region.h>
#include <zcall.h>
#include <ztypes.h>

#include "hw/gpt.h"
#include "hw/pcie.h"
#include "yellowstone_server.h"

glcr::ErrorCode SpawnProcess(z_cap_t vmmo_cap, z_cap_t yellowstone_cap) {
  mmth::OwnedMemoryRegion region =
      mmth::OwnedMemoryRegion::FromCapability(vmmo_cap);
  return mmth::SpawnProcessFromElfRegion(region.vaddr(), yellowstone_cap);
}

uint64_t main(uint64_t port_cap) {
  check(ParseInitPort(port_cap));
  dbgln("Yellowstone Initializing.");

  ASSIGN_OR_RETURN(auto server, YellowstoneServer::Create());
  Thread server_thread = server->RunServer();

  ASSIGN_OR_RETURN(uint64_t client_cap, server->CreateClientCap());
  check(SpawnProcess(gBootDenaliVmmoCap, client_cap));

  server->WaitDenaliRegistered();

  ASSIGN_OR_RETURN(client_cap, server->CreateClientCap());
  check(SpawnProcess(gBootVictoriaFallsVmmoCap, client_cap));

  server->WaitVictoriaFallsRegistered();

  dbgln("VFS Available.");

  mmth::File init_file = mmth::File::Open("/init.txt");

  glcr::Vector<glcr::StringView> files =
      glcr::StrSplit(init_file.as_str(), '\n');

  for (uint64_t i = 0; i < files.size(); i++) {
    if (!files[i].empty()) {
      mmth::File binary =
          mmth::File::Open(glcr::StrFormat("/bin/{}", files[i]));

      ASSIGN_OR_RETURN(client_cap, server->CreateClientCap());
      check(mmth::SpawnProcessFromElfRegion((uint64_t)binary.raw_ptr(),
                                            client_cap));
    }
  }

  check(server_thread.Join());
  dbgln("Yellowstone Finished Successfully.");
  return 0;
}
