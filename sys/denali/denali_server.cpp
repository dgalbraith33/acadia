#include "denali_server.h"

#include <glacier/memory/move.h>
#include <glacier/status/error.h>
#include <mammoth/util/debug.h>
#include <zcall.h>

glcr::ErrorOr<glcr::UniquePtr<DenaliServer>> DenaliServer::Create(
    AhciController& driver) {
  z_cap_t cap;
  RET_ERR(ZEndpointCreate(&cap));
  return glcr::UniquePtr<DenaliServer>(new DenaliServer(cap, driver));
}

glcr::Status DenaliServer::HandleRead(const ReadRequest& req,
                                      ReadResponse& resp) {
  ASSIGN_OR_RETURN(AhciPort * device, driver_.GetDevice(req.device_id()));

  uint64_t paddr;
  mmth::OwnedMemoryRegion region = mmth::OwnedMemoryRegion::ContiguousPhysical(
      req.block().size() * 512, &paddr);

  ASSIGN_OR_RETURN(
      auto semaphore,
      device->IssueRead(req.block().lba(), req.block().size(), paddr));
  semaphore->Wait();

  resp.set_device_id(req.device_id());
  resp.set_size(req.block().size());
  resp.set_memory(region.DuplicateCap());
  return glcr::Status::Ok();
}

glcr::Status DenaliServer::HandleReadMany(const ReadManyRequest& req,
                                          ReadResponse& resp) {
  ASSIGN_OR_RETURN(AhciPort * device, driver_.GetDevice(req.device_id()));

  uint64_t sector_cnt = 0;
  for (auto& block : req.blocks()) {
    sector_cnt += block.size();
  }
  uint64_t region_paddr;
  mmth::OwnedMemoryRegion region = mmth::OwnedMemoryRegion::ContiguousPhysical(
      sector_cnt * 512, &region_paddr);

  for (auto& block : req.blocks()) {
    ASSIGN_OR_RETURN(
        auto semaphore,
        device->IssueRead(block.lba(), block.size(), region_paddr));
    semaphore->Wait();

    region_paddr += block.size() * 512;
  }

  resp.set_device_id(req.device_id());
  resp.set_size(sector_cnt);
  resp.set_memory(region.DuplicateCap());
  return glcr::Status::Ok();
}
