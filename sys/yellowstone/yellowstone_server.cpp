#include "yellowstone_server.h"

#include <denali/denali.yunq.client.h>
#include <glacier/string/string.h>
#include <mammoth/debug.h>
#include <mammoth/init.h>
#include <mammoth/memory_region.h>
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

  ASSIGN_OR_RETURN(Mutex denali_mut, Mutex::Create());
  RET_ERR(denali_mut.Lock());

  ASSIGN_OR_RETURN(Mutex victoriafalls_mut, Mutex::Create());
  RET_ERR(victoriafalls_mut.Lock());

  return glcr::UniquePtr<YellowstoneServer>(new YellowstoneServer(
      endpoint_cap, glcr::Move(denali_mut), glcr::Move(victoriafalls_mut)));
}

YellowstoneServer::YellowstoneServer(z_cap_t endpoint_cap, Mutex&& denali_mutex,
                                     Mutex&& victoriafalls_mutex)
    : YellowstoneServerBase(endpoint_cap),
      has_denali_mutex_(glcr::Move(denali_mutex)),
      has_victoriafalls_mutex_(glcr::Move(victoriafalls_mutex)) {}

glcr::ErrorCode YellowstoneServer::HandleGetAhciInfo(const Empty&,
                                                     AhciInfo& info) {
  info.set_ahci_region(pci_reader_.GetAhciVmmo());
  info.set_region_length(kPcieConfigurationSize);
  dbgln("Resp ahci");
  return glcr::OK;
}

glcr::ErrorCode YellowstoneServer::HandleGetFramebufferInfo(
    const Empty&, FramebufferInfo& info) {
  // FIXME: Don't do this for each request.
  MappedMemoryRegion region =
      MappedMemoryRegion::FromCapability(gBootFramebufferVmmoCap);
  ZFramebufferInfo* fb = reinterpret_cast<ZFramebufferInfo*>(region.vaddr());

  info.set_address_phys(fb->address_phys);
  info.set_width(fb->width);
  info.set_height(fb->height);
  info.set_pitch(fb->pitch);
  info.set_bpp(fb->bpp);
  info.set_memory_model(fb->memory_model);
  info.set_red_mask_size(fb->red_mask_size);
  info.set_red_mask_shift(fb->red_mask_shift);
  info.set_green_mask_size(fb->green_mask_size);
  info.set_green_mask_shift(fb->green_mask_shift);
  info.set_blue_mask_size(fb->blue_mask_size);
  info.set_blue_mask_shift(fb->blue_mask_shift);

  return glcr::OK;
}

glcr::ErrorCode YellowstoneServer::HandleGetDenali(const Empty&,
                                                   DenaliInfo& info) {
  z_cap_t new_denali;
  check(ZCapDuplicate(denali_cap_, kZionPerm_All, &new_denali));
  info.set_denali_endpoint(new_denali);
  info.set_device_id(device_id_);
  info.set_lba_offset(lba_offset_);
  dbgln("Resp denali");
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
    // FIXME: Probably make a separate copy for use within yellowstone vs
    // transmit to other processes.
    vfs_client_ = glcr::MakeShared<VFSClient>(victoria_falls_cap_);
    check(has_victoriafalls_mutex_.Release());
  } else {
    dbgln("[WARN] Got endpoint cap type: {}", req.endpoint_name().cstr());
  }
  return glcr::OK;
}

glcr::ErrorCode YellowstoneServer::WaitDenaliRegistered() {
  RET_ERR(has_denali_mutex_.Lock());
  return has_denali_mutex_.Release();
}

glcr::ErrorCode YellowstoneServer::WaitVictoriaFallsRegistered() {
  RET_ERR(has_victoriafalls_mutex_.Lock());
  return has_victoriafalls_mutex_.Release();
}

glcr::SharedPtr<VFSClient> YellowstoneServer::GetVFSClient() {
  return vfs_client_;
}
