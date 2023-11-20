#include "denali_server.h"

#include <glacier/memory/move.h>
#include <glacier/status/error.h>
#include <mammoth/debug.h>
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
  ASSIGN_OR_RETURN(Mutex mutex, Mutex::Create());
  RET_ERR(mutex.Lock());

  uint64_t paddr;
  OwnedMemoryRegion region =
      OwnedMemoryRegion::ContiguousPhysical(req.size() * 512, &paddr);

  DmaReadCommand command(req.lba(), req.size(), paddr, mutex);
  device->IssueCommand(&command);

  // Wait for read operation to complete.
  RET_ERR(mutex.Lock());
  RET_ERR(mutex.Release());

  resp.set_device_id(req.device_id());
  resp.set_size(req.size());
  resp.set_memory(region.DuplicateCap());
  return glcr::OK;
}

glcr::ErrorCode DenaliServer::HandleReadMany(const ReadManyRequest& req,
                                             ReadResponse& resp) {
  ASSIGN_OR_RETURN(AhciDevice * device, driver_.GetDevice(req.device_id()));
  ASSIGN_OR_RETURN(Mutex mutex, Mutex::Create());
  RET_ERR(mutex.Lock());

  uint64_t region_paddr;
  OwnedMemoryRegion region = OwnedMemoryRegion::ContiguousPhysical(
      req.lba().size() * 512, &region_paddr);

  auto& vec = req.lba();
  uint64_t curr_run_start = 0;
  for (uint64_t i = 0; i < vec.size(); i++) {
    if (i + 1 < vec.size() && vec.at(i) + 1 == vec.at(i + 1)) {
      continue;
    }
    uint64_t lba = vec.at(curr_run_start);
    uint64_t size = (i - curr_run_start) + 1;
    uint64_t paddr = region_paddr + curr_run_start * 512;
    DmaReadCommand command(lba, size, paddr, mutex);
    device->IssueCommand(&command);

    // Wait for read operation to complete.
    RET_ERR(mutex.Lock());

    curr_run_start = i + 1;
  }

  resp.set_device_id(req.device_id());
  resp.set_size(req.lba().size());
  resp.set_memory(region.DuplicateCap());
  return glcr::OK;
}
