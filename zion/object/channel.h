#pragma once

#include <glacier/container/pair.h>
#include <glacier/memory/ref_ptr.h>

#include "capability/capability.h"
#include "include/ztypes.h"
#include "lib/message_queue.h"
#include "lib/mutex.h"
#include "object/ipc_object.h"
#include "object/kernel_object.h"
#include "usr/zcall_internal.h"

class Channel;

template <>
struct KernelObjectTag<Channel> {
  static const uint64_t type = KernelObject::CHANNEL;
};

class Channel : public IpcObject {
 public:
  uint64_t TypeTag() override { return KernelObject::CHANNEL; }
  static glcr::Pair<glcr::RefPtr<Channel>, glcr::RefPtr<Channel>>
  CreateChannelPair();

  glcr::RefPtr<Channel> peer() { return peer_; }

  virtual MessageQueue& GetSendMessageQueue() override {
    return peer_->message_queue_;
  }
  virtual MessageQueue& GetRecvMessageQueue() override {
    return message_queue_;
  }

 private:
  // FIXME: We will likely never close the channel based on this
  // circular dependency.
  glcr::RefPtr<Channel> peer_{nullptr};

  UnboundedMessageQueue message_queue_;

  Channel() {}
  void SetPeer(const glcr::RefPtr<Channel>& peer) { peer_ = peer; }
};
