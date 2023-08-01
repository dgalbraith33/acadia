#include "denali_server.h"

#include <glacier/memory/move.h>
#include <glacier/status/error.h>
#include <mammoth/debug.h>
#include <zcall.h>

namespace {
DenaliServer* gServer = nullptr;
void HandleResponse(ResponseContext& response, uint64_t lba, uint64_t size,
                    z_cap_t mem) {
  gServer->HandleResponse(response, lba, size, mem);
}
}  // namespace

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

  device->IssueCommand(new DmaReadCommand(request->lba, request->size,
                                          ::HandleResponse, context));

  return glcr::OK;
}

void DenaliServer::HandleResponse(ResponseContext& response, uint64_t lba,
                                  uint64_t size, z_cap_t mem) {
  DenaliReadResponse resp{
      .device_id = 0,
      .lba = lba,
      .size = size,
  };
  check(response.WriteStructWithCap<DenaliReadResponse>(resp, mem));
}
