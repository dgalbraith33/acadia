#include "yellowstone_server.h"

#include <denali/denali.yunq.client.h>
#include <glacier/string/string.h>
#include <mammoth/file/file.h>
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>
#include <mammoth/util/memory_region.h>
#include <zcall.h>

#include "hw/gpt.h"
#include "hw/pcie.h"

namespace yellowstone {
namespace {

struct PartitionInfo {
  uint64_t device_id;
  uint64_t partition_lba;
};

glcr::ErrorOr<PartitionInfo> HandleDenaliRegistration(z_cap_t endpoint_cap) {
  GptReader reader(
      glcr::UniquePtr<DenaliClient>(new DenaliClient(endpoint_cap)));

  auto status = reader.ParsePartitionTables();
  if (!status.ok()) {
    dbgln("GPT Reader: {}", status.message());
    return status.code();
  }

  return PartitionInfo{.device_id = 0,
                       .partition_lba = reader.GetPrimaryPartitionLba()};
}

}  // namespace

glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> YellowstoneServer::Create() {
  z_cap_t endpoint_cap;
  RET_ERR(ZEndpointCreate(&endpoint_cap));

  return glcr::UniquePtr<YellowstoneServer>(
      new YellowstoneServer(endpoint_cap));
}

YellowstoneServer::YellowstoneServer(z_cap_t endpoint_cap)
    : YellowstoneServerBase(endpoint_cap) {}

glcr::Status YellowstoneServer::HandleGetAhciInfo(AhciInfo& info) {
  info.set_ahci_region(pci_reader_.GetAhciVmmo());
  info.set_region_length(kPcieConfigurationSize);
  return glcr::Status::Ok();
}

glcr::Status YellowstoneServer::HandleGetFramebufferInfo(
    FramebufferInfo& info) {
  // FIXME: Don't do this for each request.
  mmth::OwnedMemoryRegion region =
      mmth::OwnedMemoryRegion::FromCapability(gBootFramebufferVmmoCap);
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

  return glcr::Status::Ok();
}

glcr::Status YellowstoneServer::HandleGetDenali(DenaliInfo& info) {
  if (!endpoint_map_.Contains("denali")) {
    return glcr::NotFound("Denali Capability Not registered");
  }
  z_cap_t new_denali;
  check(ZCapDuplicate(endpoint_map_.at("denali"), kZionPerm_All, &new_denali));
  info.set_denali_endpoint(new_denali);
  info.set_device_id(device_id_);
  info.set_lba_offset(lba_offset_);
  return glcr::Status::Ok();
}

glcr::Status YellowstoneServer::HandleRegisterEndpoint(
    const RegisterEndpointRequest& req) {
  dbgln("Registering {}.", req.endpoint_name().view());
  check(endpoint_map_.Insert(req.endpoint_name(), req.endpoint_capability()));
  if (req.endpoint_name() == "denali") {
    z_cap_t dup_cap;
    check(ZCapDuplicate(req.endpoint_capability(), kZionPerm_All, &dup_cap));
    auto part_info_or = HandleDenaliRegistration(dup_cap);
    if (!part_info_or.ok()) {
      check(part_info_or.error());
    }
    device_id_ = part_info_or.value().device_id;
    lba_offset_ = part_info_or.value().partition_lba;

    has_denali_semaphore_.Signal();
  } else if (req.endpoint_name() == "victoriafalls") {
    // FIXME: Probably make a separate copy for use within yellowstone vs
    // transmit to other processes.
    mmth::SetVfsCap(req.endpoint_capability());
    has_victoriafalls_semaphore_.Signal();
  } else {
    dbgln("[WARN] Got endpoint cap type: {}", req.endpoint_name().cstr());
  }
  return glcr::Status::Ok();
}

glcr::Status YellowstoneServer::HandleGetEndpoint(const GetEndpointRequest& req,
                                                  Endpoint& resp) {
  if (!endpoint_map_.Contains(req.endpoint_name())) {
    return glcr::NotFound(
        glcr::StrFormat("Endpoint '{}' not found.", req.endpoint_name()));
  }
  z_cap_t cap = endpoint_map_.at(req.endpoint_name());
  z_cap_t new_cap;
  check(ZCapDuplicate(cap, kZionPerm_All, &new_cap));
  resp.set_endpoint(new_cap);
  return glcr::Status::Ok();
}

void YellowstoneServer::WaitDenaliRegistered() { has_denali_semaphore_.Wait(); }

void YellowstoneServer::WaitVictoriaFallsRegistered() {
  has_victoriafalls_semaphore_.Wait();
}

}  // namespace yellowstone
