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

  DmaReadCommand command(req.lba(), req.size(), mutex);
  device->IssueCommand(&command);

  // Wait for read operation to complete.
  RET_ERR(mutex.Lock());
  RET_ERR(mutex.Release());

  resp.set_device_id(req.device_id());
  resp.set_lba(req.lba());
  resp.set_size(req.size());
  resp.set_memory(command.GetMemoryRegion());
  return glcr::OK;
}
