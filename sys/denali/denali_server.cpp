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

glcr::ErrorCode DenaliServer::HandleRequest(RequestContext& request,
                                            ResponseContext& response) {
  switch (request.request_id()) {
    case DENALI_READ: {
      DenaliReadRequest* req = 0;
      glcr::ErrorCode err = request.As<DenaliReadRequest>(&req);
      if (err != glcr::OK) {
        response.WriteError(err);
      }
      err = HandleRead(req, response);
      if (err != glcr::OK) {
        response.WriteError(err);
      }
      break;
    }
    default:
      response.WriteError(glcr::UNIMPLEMENTED);
      break;
  }
  return glcr::OK;
}

glcr::ErrorCode DenaliServer::HandleRead(DenaliReadRequest* request,
                                         ResponseContext& context) {
  ASSIGN_OR_RETURN(AhciDevice * device, driver_.GetDevice(request->device_id));
  ASSIGN_OR_RETURN(Mutex mutex, Mutex::Create());
  RET_ERR(mutex.Lock());

  DmaReadCommand command(request->lba, request->size, mutex, context);
  device->IssueCommand(&command);

  // Wait for read operation to complete.
  RET_ERR(mutex.Lock());
  RET_ERR(mutex.Release());
  DenaliReadResponse resp{
      .device_id = request->device_id,
      .lba = request->lba,
      .size = request->size,
  };
  return context.WriteStructWithCap<DenaliReadResponse>(
      resp, command.GetMemoryRegion());
}
