#include "syscall/endpoint.h"

#include "object/endpoint.h"
#include "object/reply_port.h"
#include "scheduler/scheduler.h"

glcr::ErrorCode EndpointCreate(ZEndpointCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  *req->endpoint_cap =
      proc.AddNewCapability(Endpoint::Create(), ZC_READ | ZC_WRITE);
  return glcr::OK;
}

glcr::ErrorCode EndpointSend(ZEndpointSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto endpoint_cap = proc.GetCapability(req->endpoint_cap);
  ValidateCapability<Endpoint>(endpoint_cap, ZC_WRITE);
  auto endpoint = endpoint_cap->obj<Endpoint>();

  auto reply_port = ReplyPort::Create();
  *req->reply_port_cap = proc.AddNewCapability(reply_port, ZC_READ);
  uint64_t reply_port_cap_to_send = proc.AddNewCapability(reply_port, ZC_WRITE);
  return endpoint->Write(req->num_bytes, req->data, reply_port_cap_to_send);
}

glcr::ErrorCode EndpointRecv(ZEndpointRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto endpoint_cap = proc.GetCapability(req->endpoint_cap);
  ValidateCapability<Endpoint>(endpoint_cap, ZC_READ);
  auto endpoint = endpoint_cap->obj<Endpoint>();

  return endpoint->Read(req->num_bytes, req->data, req->reply_port_cap);
}
