#include "syscall/ipc.h"

#include "capability/capability.h"
#include "debug/debug.h"
#include "interrupt/driver_manager.h"
#include "interrupt/interrupt.h"
#include "object/endpoint.h"
#include "object/reply_port.h"
#include "scheduler/scheduler.h"

namespace {

glcr::ArrayView<uint8_t> Buffer(const void* bytes, uint64_t num_bytes) {
  return glcr::ArrayView(reinterpret_cast<uint8_t*>(const_cast<void*>(bytes)),
                         num_bytes);
}

template <typename T>
glcr::ErrorOr<IpcMessage> TranslateRequestToIpcMessage(const T& req) {
  if (req.num_bytes > 0x1000) {
    dbgln("Large message size unimplemented: {x}", req.num_bytes);
    return glcr::UNIMPLEMENTED;
  }

  IpcMessage message;
  message.data = Buffer(req.data, req.num_bytes);

  glcr::ArrayView<const z_cap_t> caps(req.caps, req.num_caps);

  message.caps.Resize(caps.size());
  for (uint64_t capid : caps) {
    // FIXME: This would feel safer closer to the relevant syscall.
    // FIXME: Race conditions on get->check->release here. Would be better to
    // have that as a single call on the process. (This pattern repeats other
    // places too).
    auto cap = gScheduler->CurrentProcess().GetCapability(capid);
    if (!cap) {
      return glcr::CAP_NOT_FOUND;
    }
    if (!cap->HasPermissions(kZionPerm_Transmit)) {
      return glcr::CAP_PERMISSION_DENIED;
    }
    message.caps.PushBack(
        gScheduler->CurrentProcess().ReleaseCapability(capid));
  }

  return message;
}

template <typename T>
glcr::ErrorCode TranslateIpcMessageToResponse(const IpcMessage& message,
                                              T* resp) {
  if (message.data.size() > *resp->num_bytes) {
    return glcr::BUFFER_SIZE;
  }
  if (message.caps.size() > *resp->num_caps) {
    return glcr::BUFFER_SIZE;
  }

  *resp->num_bytes = message.data.size();
  for (uint64_t i = 0; i < message.data.size(); i++) {
    reinterpret_cast<uint8_t*>(resp->data)[i] = message.data[i];
  }

  *resp->num_caps = message.caps.size();
  auto& proc = gScheduler->CurrentProcess();
  for (uint64_t i = 0; i < *resp->num_caps; i++) {
    resp->caps[i] = proc.AddExistingCapability(message.caps[i]);
  }
  return glcr::OK;
}

template <typename T>
glcr::ErrorCode TranslateIpcMessageToResponseWithReplyPort(
    const IpcMessage& message, T* resp) {
  TranslateIpcMessageToResponse(message, resp);

  if (!message.reply_cap) {
    dbgln("Tried to read reply capability off of a message without one");
    return glcr::INTERNAL;
  }
  auto& proc = gScheduler->CurrentProcess();
  *resp->reply_port_cap = proc.AddExistingCapability(message.reply_cap);
  return glcr::OK;
}

}  // namespace

glcr::ErrorCode ChannelCreate(ZChannelCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_pair = Channel::CreateChannelPair();
  *req->channel1 = proc.AddNewCapability(chan_pair.first());
  *req->channel2 = proc.AddNewCapability(chan_pair.second());
  return glcr::OK;
}

glcr::ErrorCode ChannelSend(ZChannelSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCapability<Channel>(chan_cap, kZionPerm_Write));

  auto chan = chan_cap->obj<Channel>();
  ASSIGN_OR_RETURN(IpcMessage message, TranslateRequestToIpcMessage(*req));
  return chan->Send(glcr::Move(message));
}

glcr::ErrorCode ChannelRecv(ZChannelRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCapability<Channel>(chan_cap, kZionPerm_Read));

  auto chan = chan_cap->obj<Channel>();
  ASSIGN_OR_RETURN(IpcMessage msg, chan->Recv(*req->num_bytes, *req->num_caps));
  return TranslateIpcMessageToResponse(msg, req);
}

glcr::ErrorCode PortCreate(ZPortCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port = glcr::MakeRefCounted<Port>();
  *req->port_cap = proc.AddNewCapability(port);
  return glcr::OK;
}

glcr::ErrorCode PortSend(ZPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, kZionPerm_Write));

  auto port = port_cap->obj<Port>();
  ASSIGN_OR_RETURN(IpcMessage message, TranslateRequestToIpcMessage(*req));
  return port->Send(glcr::Move(message));
}

glcr::ErrorCode PortRecv(ZPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, kZionPerm_Read));

  auto port = port_cap->obj<Port>();
  ASSIGN_OR_RETURN(IpcMessage msg, port->Recv(*req->num_bytes, *req->num_caps));
  return TranslateIpcMessageToResponse(msg, req);
}

glcr::ErrorCode PortPoll(ZPortPollReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto port_cap = proc.GetCapability(req->port_cap);
  RET_ERR(ValidateCapability<Port>(port_cap, kZionPerm_Read));

  auto port = port_cap->obj<Port>();
  // FIXME: Race condition here where this call could block if the last message
  // is removed between this check and the port read.
  if (!port->HasMessages()) {
    return glcr::EMPTY;
  }
  ASSIGN_OR_RETURN(IpcMessage msg, port->Recv(*req->num_bytes, *req->num_caps));
  return TranslateIpcMessageToResponse(msg, req);
}

glcr::ErrorCode IrqRegister(ZIrqRegisterReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  glcr::RefPtr<Port> port = glcr::MakeRefCounted<Port>();

  *req->port_cap = proc.AddNewCapability(port);
  return DriverManager::Get().RegisterListener(req->irq_num, port);
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

  ASSIGN_OR_RETURN(IpcMessage message, TranslateRequestToIpcMessage(*req));
  message.reply_cap = glcr::MakeRefCounted<Capability>(
      reply_port, kZionPerm_Write | kZionPerm_Transmit);
  return endpoint->Send(glcr::Move(message));
}

glcr::ErrorCode EndpointRecv(ZEndpointRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto endpoint_cap = proc.GetCapability(req->endpoint_cap);
  ValidateCapability<Endpoint>(endpoint_cap, kZionPerm_Read);
  auto endpoint = endpoint_cap->obj<Endpoint>();

  ASSIGN_OR_RETURN(IpcMessage msg,
                   endpoint->Recv(*req->num_bytes, *req->num_caps));
  return TranslateIpcMessageToResponseWithReplyPort(msg, req);
}

glcr::ErrorCode ReplyPortSend(ZReplyPortSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto reply_port_cap = proc.GetCapability(req->reply_port_cap);
  ValidateCapability<ReplyPort>(reply_port_cap, kZionPerm_Read);
  auto reply_port = reply_port_cap->obj<ReplyPort>();

  ASSIGN_OR_RETURN(IpcMessage message, TranslateRequestToIpcMessage(*req));
  return reply_port->Send(glcr::Move(message));
}
glcr::ErrorCode ReplyPortRecv(ZReplyPortRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();

  auto reply_port_cap = proc.GetCapability(req->reply_port_cap);
  ValidateCapability<ReplyPort>(reply_port_cap, kZionPerm_Read);
  auto reply_port = reply_port_cap->obj<ReplyPort>();

  ASSIGN_OR_RETURN(IpcMessage msg,
                   reply_port->Recv(*req->num_bytes, *req->num_caps));
  return TranslateIpcMessageToResponse(msg, req);
}
