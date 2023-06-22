#include "syscall/reply_port.h"

#include "object/reply_port.h"
#include "scheduler/scheduler.h"

glcr::ErrorCode ReplyPortSend(ZReplyPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto reply_port_cap = proc.GetCapability(req->reply_port_cap);
  ValidateCapability<ReplyPort>(reply_port_cap, ZC_WRITE);
  auto reply_port = reply_port_cap->obj<ReplyPort>();

  return reply_port->Write(req->num_bytes, req->data, req->num_caps, req->caps);
}
glcr::ErrorCode ReplyPortRecv(ZReplyPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto reply_port_cap = proc.GetCapability(req->reply_port_cap);
  ValidateCapability<ReplyPort>(reply_port_cap, ZC_READ);
  auto reply_port = reply_port_cap->obj<ReplyPort>();

  return reply_port->Read(req->num_bytes, req->data, req->num_caps, req->caps);
}
