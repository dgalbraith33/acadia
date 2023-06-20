#include "syscall/channel.h"

#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

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
  RET_ERR(ValidateCap(chan_cap, ZC_WRITE));

  auto chan = chan_cap->obj<Channel>();
  RET_IF_NULL(chan);
  // FIXME: Get rid of this hack.
  ZMessage message{
      .num_bytes = req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = req->num_caps,
      .caps = req->caps,

  };
  return chan->Write(message);
}

z_err_t ChannelRecv(ZChannelRecvReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto chan_cap = proc.GetCapability(req->chan_cap);
  RET_ERR(ValidateCap(chan_cap, ZC_READ));

  auto chan = chan_cap->obj<Channel>();
  RET_IF_NULL(chan);
  // FIXME: Get rid of this hack.
  ZMessage message{
      .num_bytes = *req->num_bytes,
      .data = const_cast<void*>(req->data),
      .num_caps = *req->num_caps,
      .caps = req->caps,

  };
  RET_ERR(chan->Read(message));
  *req->num_bytes = message.num_bytes;
  *req->num_caps = message.num_caps;
  return Z_OK;
}
