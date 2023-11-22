#include "denali_server.h"

#include <glacier/memory/move.h>
#include <glacier/status/error.h>
#include <mammoth/util/debug.h>
#include <zcall.h>

glcr::ErrorOr<glcr::UniquePtr<DenaliServer>> DenaliServer::Create(
    AhciDriver& driver) {
  z_cap_t cap;
  RET_ERR(ZEndpointCreate(&cap));
  return glcr::UniquePtr<DenaliServer>(new DenaliServer(cap, driver));
}

glcr::ErrorCode DenaliServer::HandleRead(const ReadRequest& req,
                                         ReadResponse& resp) {
  ASSIGN_OR_RETURN(AhciDevice * device, driver_.GetDevice(req.device_id()));

  uint64_t paddr;
  OwnedMemoryRegion region =
      OwnedMemoryRegion::ContiguousPhysical(req.size() * 512, &paddr);

  DmaReadCommand command(req.lba(), req.size(), paddr);
  device->IssueCommand(&command);

  command.WaitComplete();

  resp.set_device_id(req.device_id());
  resp.set_size(req.size());
  resp.set_memory(region.DuplicateCap());
  return glcr::OK;
}

glcr::ErrorCode DenaliServer::HandleReadMany(const ReadManyRequest& req,
                                             ReadResponse& resp) {
  ASSIGN_OR_RETURN(AhciDevice * device, driver_.GetDevice(req.device_id()));

  if (req.lba().size() != req.sector_cnt().size()) {
    return glcr::INVALID_ARGUMENT;
  }

  uint64_t sector_cnt = 0;
  for (uint64_t i = 0; i < req.sector_cnt().size(); i++) {
    sector_cnt += req.sector_cnt().at(i);
  }
  uint64_t region_paddr;
  OwnedMemoryRegion region =
      OwnedMemoryRegion::ContiguousPhysical(sector_cnt * 512, &region_paddr);

  for (uint64_t i = 0; i < req.lba().size(); i++) {
    uint64_t lba = req.lba().at(i);
    uint64_t size = req.sector_cnt().at(i);
    DmaReadCommand command(lba, size, region_paddr);
    device->IssueCommand(&command);
    command.WaitComplete();

    region_paddr += size * 512;
  }

  resp.set_device_id(req.device_id());
  resp.set_size(sector_cnt);
  resp.set_memory(region.DuplicateCap());
  return glcr::OK;
}
