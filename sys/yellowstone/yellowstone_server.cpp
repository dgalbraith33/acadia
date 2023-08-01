#include "yellowstone_server.h"

#include <denali/denali.h>
#include <glacier/string/string.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <stdlib.h>

#include "hw/gpt.h"
#include "include/yellowstone.h"

namespace {

void RegistrationThreadBootstrap(void* yellowstone) {
  dbgln("Yellowstone registration starting");
  static_cast<YellowstoneServer*>(yellowstone)->RegistrationThread();
}

struct PartitionInfo {
  uint64_t device_id;
  uint64_t partition_lba;
};

glcr::ErrorOr<PartitionInfo> HandleDenaliRegistration(z_cap_t endpoint_cap) {
  GptReader reader(glcr::UniquePtr<DenaliClient>(
      new DenaliClient(EndpointClient::AdoptEndpoint(endpoint_cap))));

  RET_ERR(reader.ParsePartitionTables());

  return PartitionInfo{.device_id = 0,
                       .partition_lba = reader.GetPrimaryPartitionLba()};
}

}  // namespace

glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> YellowstoneServer::Create() {
  z_cap_t cap;
  RET_ERR(ZEndpointCreate(&cap));
  ASSIGN_OR_RETURN(PortServer port, PortServer::Create());
  return glcr::UniquePtr<YellowstoneServer>(new YellowstoneServer(cap, port));
}

YellowstoneServer::YellowstoneServer(z_cap_t endpoint_cap, PortServer port)
    : EndpointServer(endpoint_cap), register_port_(port) {}

Thread YellowstoneServer::RunRegistration() {
  return Thread(RegistrationThreadBootstrap, this);
}

glcr::ErrorCode YellowstoneServer::HandleRequest(RequestContext& request,
                                                 ResponseContext& response) {
  switch (request.request_id()) {
    case kYellowstoneGetAhci: {
      dbgln("Yellowstone::GetAHCI");
      YellowstoneGetAhciResp resp{
          .type = kYellowstoneGetAhci,
          .ahci_phys_offset = pci_reader_.GetAhciPhysical(),
      };
      RET_ERR(response.WriteStruct<YellowstoneGetAhciResp>(resp));
      break;
    }
    case kYellowstoneGetRegistration: {
      dbgln("Yellowstone::GetRegistration");
      auto client_or = register_port_.CreateClient();
      if (!client_or.ok()) {
        check(client_or.error());
      }
      YellowstoneGetRegistrationResp resp;
      uint64_t reg_cap = client_or.value().cap();
      RET_ERR(response.WriteStructWithCap(resp, reg_cap));
      break;
    }
    case kYellowstoneGetDenali: {
      dbgln("Yellowstone::GetDenali");
      z_cap_t new_denali;
      check(ZCapDuplicate(denali_cap_, &new_denali));
      YellowstoneGetDenaliResp resp{
          .type = kYellowstoneGetDenali,
          .device_id = device_id_,
          .lba_offset = lba_offset_,
      };
      RET_ERR(response.WriteStructWithCap(resp, new_denali));
      break;
    }
    default:
      dbgln("Unknown request type: %x", request.request_id());
      return glcr::UNIMPLEMENTED;
      break;
  }
  return glcr::OK;
}

void YellowstoneServer::RegistrationThread() {
  while (true) {
    uint64_t num_bytes = kBufferSize;
    z_cap_t endpoint_cap;
    // FIXME: Better error handling here.
    check(register_port_.RecvCap(&num_bytes, registration_buffer_,
                                 &endpoint_cap));
    glcr::String name(registration_buffer_);
    if (name == "denali") {
      denali_cap_ = endpoint_cap;
      auto part_info_or = HandleDenaliRegistration(denali_cap_);
      if (!part_info_or.ok()) {
        check(part_info_or.error());
      }
      device_id_ = part_info_or.value().device_id;
      lba_offset_ = part_info_or.value().partition_lba;

      uint64_t vaddr;
      check(
          ZAddressSpaceMap(gSelfVmasCap, 0, gBootVictoriaFallsVmmoCap, &vaddr));
      auto client_or = CreateClient();
      if (!client_or.ok()) {
        check(client_or.error());
      }
      check(SpawnProcessFromElfRegion(vaddr, glcr::Move(client_or.value())));
      continue;
    }

    if (name == "victoriafalls") {
      victoria_falls_cap_ = endpoint_cap;
      continue;
    }

    dbgln("[WARN] Got endpoint cap type:");
    dbgln(name.cstr());
  }
}
