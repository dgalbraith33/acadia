#include "syscall/channel.h"

#include "capability/capability.h"
#include "scheduler/scheduler.h"

z_err_t ChannelCreate(ZChannelCreateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_pair = Channel::CreateChannelPair();
  *req->channel1 = proc.AddNewCapability(chan_pair.first(), ZC_WRITE | ZC_READ);
  *req->channel2 =
      proc.AddNewCapability(chan_pair.second(), ZC_WRITE | ZC_READ);
  return Z_OK;
}

z_err_t ChannelSend(ZChannelSendReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCapability<Channel>(chan_cap, ZC_WRITE));

  auto chan = chan_cap->obj<Channel>();
  return chan->Write(req->num_bytes, req->data, req->num_caps, req->caps);
}

z_err_t ChannelRecv(ZChannelRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCapability<Channel>(chan_cap, ZC_READ));

  auto chan = chan_cap->obj<Channel>();
  return chan->Read(req->num_bytes, req->data, req->num_caps, req->caps);
}
