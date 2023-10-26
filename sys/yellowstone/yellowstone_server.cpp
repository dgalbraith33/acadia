#include "yellowstone_server.h"

#include <denali/denali.yunq.client.h>
#include <glacier/string/string.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/process.h>
#include <stdlib.h>

#include "hw/gpt.h"
#include "hw/pcie.h"

namespace {

struct PartitionInfo {
  uint64_t device_id;
  uint64_t partition_lba;
};

glcr::ErrorOr<PartitionInfo> HandleDenaliRegistration(z_cap_t endpoint_cap) {
  GptReader reader(
      glcr::UniquePtr<DenaliClient>(new DenaliClient(endpoint_cap)));

  RET_ERR(reader.ParsePartitionTables());

  return PartitionInfo{.device_id = 0,
                       .partition_lba = reader.GetPrimaryPartitionLba()};
}

}  // namespace

glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> YellowstoneServer::Create() {
  z_cap_t endpoint_cap;
  RET_ERR(ZEndpointCreate(&endpoint_cap));

  ASSIGN_OR_RETURN(Mutex mut, Mutex::Create());
  RET_ERR(mut.Lock());

  return glcr::UniquePtr<YellowstoneServer>(
      new YellowstoneServer(endpoint_cap, glcr::Move(mut)));
}

YellowstoneServer::YellowstoneServer(z_cap_t endpoint_cap, Mutex&& mutex)
    : YellowstoneServerBase(endpoint_cap),
      has_denali_mutex_(glcr::Move(mutex)) {}

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

glcr::ErrorCode YellowstoneServer::HandleRegisterEndpoint(
    const RegisterEndpointRequest& req, Empty&) {
  dbgln("Registering.");
  if (req.endpoint_name() == "denali") {
    // FIXME: Rather than blocking and calling the denali service
    // immediately we should signal the main thread that it can continue init.
    denali_cap_ = req.endpoint_capability();
    auto part_info_or = HandleDenaliRegistration(denali_cap_);
    if (!part_info_or.ok()) {
      check(part_info_or.error());
    }
    device_id_ = part_info_or.value().device_id;
    lba_offset_ = part_info_or.value().partition_lba;

    check(has_denali_mutex_.Release());
  } else if (req.endpoint_name() == "victoriafalls") {
    victoria_falls_cap_ = req.endpoint_capability();
  } else {
    dbgln("[WARN] Got endpoint cap type: %s", req.endpoint_name().cstr());
  }
  return glcr::OK;
}

glcr::ErrorCode YellowstoneServer::WaitDenaliRegistered() {
  RET_ERR(has_denali_mutex_.Lock());
  return has_denali_mutex_.Release();
}
