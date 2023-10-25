#include "yellowstone_server.h"

#include <denali/denali.h>
#include <glacier/string/string.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <stdlib.h>

#include "hw/gpt.h"
#include "hw/pcie.h"
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
    : YellowstoneServerBase(endpoint_cap), register_port_(port) {}

Thread YellowstoneServer::RunRegistration() {
  return Thread(RegistrationThreadBootstrap, this);
}

glcr::ErrorCode YellowstoneServer::HandleGetAhciInfo(const Empty&,
                                                     AhciInfo& info) {
  info.set_ahci_region(pci_reader_.GetAhciVmmo());
  info.set_region_length(kPcieConfigurationSize);
  return glcr::OK;
}

glcr::ErrorCode YellowstoneServer::HandleGetDenali(const Empty&,
                                                   DenaliInfo& info) {
  z_cap_t new_denali;
  check(ZCapDuplicate(denali_cap_, &new_denali));
  info.set_denali_endpoint(new_denali);
  info.set_device_id(device_id_);
  info.set_lba_offset(lba_offset_);
  return glcr::OK;
}
glcr::ErrorCode YellowstoneServer::HandleGetRegister(const Empty&,
                                                     RegisterInfo& info) {
  auto client_or = register_port_.CreateClient();
  if (!client_or.ok()) {
    dbgln("Error creating register client: %u", client_or.error());
    return glcr::INTERNAL;
  }
  info.set_register_port(client_or.value().cap());
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
      check(SpawnProcessFromElfRegion(vaddr, client_or.value().Capability()));
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
