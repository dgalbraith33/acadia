#include "syscall/ipc.h"

#include "capability/capability.h"
#include "interrupt/interrupt.h"
#include "object/endpoint.h"
#include "object/reply_port.h"
#include "scheduler/scheduler.h"

z_err_t ChannelCreate(ZChannelCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_pair = Channel::CreateChannelPair();
  *req->channel1 = proc.AddNewCapability(chan_pair.first());
  *req->channel2 = proc.AddNewCapability(chan_pair.second());
  return glcr::OK;
}

z_err_t ChannelSend(ZChannelSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCapability<Channel>(chan_cap, kZionPerm_Write));

  auto chan = chan_cap->obj<Channel>();
  return chan->Send(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t ChannelRecv(ZChannelRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCapability<Channel>(chan_cap, kZionPerm_Read));

  auto chan = chan_cap->obj<Channel>();
  return chan->Recv(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t PortCreate(ZPortCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port = glcr::MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port);
  return glcr::OK;
}

z_err_t PortSend(ZPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, kZionPerm_Write));

  auto port = port_cap->obj<Port>();
  return port->Send(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t PortRecv(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, kZionPerm_Read));

  auto port = port_cap->obj<Port>();
  ZMessage message{
      .num_bytes = *req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = *req->num_caps,
      .caps = req->caps,
  };
  return port->Recv(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t PortPoll(ZPortPollReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, kZionPerm_Read));

  auto port = port_cap->obj<Port>();
  // FIXME: Race condition here where this call could block if the last message
  // is removed between this check and the port read.
  if (!port->HasMessages()) {
    return glcr::EMPTY;
  }
  return port->Recv(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t IrqRegister(ZIrqRegisterReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  if (req->irq_num != Z_IRQ_PCI_BASE) {
    // FIXME: Don't hardcode this nonsense.
    return glcr::UNIMPLEMENTED;
  }
  glcr::RefPtr<Port> port = glcr::MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port);
  RegisterPciPort(port);
  return glcr::OK;
}

glcr::ErrorCode EndpointCreate(ZEndpointCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  *req->endpoint_cap = proc.AddNewCapability(Endpoint::Create());
  return glcr::OK;
}

glcr::ErrorCode EndpointSend(ZEndpointSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto endpoint_cap = proc.GetCapability(req->endpoint_cap);
  ValidateCapability<Endpoint>(endpoint_cap, kZionPerm_Write);
  auto endpoint = endpoint_cap->obj<Endpoint>();

  auto reply_port = ReplyPort::Create();
  *req->reply_port_cap = proc.AddNewCapability(reply_port, kZionPerm_Read);
  uint64_t reply_port_cap_to_send =
      proc.AddNewCapability(reply_port, kZionPerm_Write | kZionPerm_Transmit);
  return endpoint->Send(req->num_bytes, req->data, req->num_caps, req->caps,
                        reply_port_cap_to_send);
}

glcr::ErrorCode EndpointRecv(ZEndpointRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto endpoint_cap = proc.GetCapability(req->endpoint_cap);
  ValidateCapability<Endpoint>(endpoint_cap, kZionPerm_Read);
  auto endpoint = endpoint_cap->obj<Endpoint>();

  uint64_t num_caps = 1;
  RET_ERR(endpoint->Recv(req->num_bytes, req->data, req->num_caps, req->caps,
                         req->reply_port_cap));
  if (num_caps != 1) {
    return glcr::INTERNAL;
  }
  return glcr::OK;
}

glcr::ErrorCode ReplyPortSend(ZReplyPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto reply_port_cap = proc.GetCapability(req->reply_port_cap);
  ValidateCapability<ReplyPort>(reply_port_cap, kZionPerm_Read);
  auto reply_port = reply_port_cap->obj<ReplyPort>();

  return reply_port->Send(req->num_bytes, req->data, req->num_caps, req->caps);
}
glcr::ErrorCode ReplyPortRecv(ZReplyPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto reply_port_cap = proc.GetCapability(req->reply_port_cap);
  ValidateCapability<ReplyPort>(reply_port_cap, kZionPerm_Read);
  auto reply_port = reply_port_cap->obj<ReplyPort>();

  return reply_port->Recv(req->num_bytes, req->data, req->num_caps, req->caps);
}
