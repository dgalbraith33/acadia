#pragma once

#include <glacier/memory/ref_ptr.h>

#include "lib/message_queue.h"
#include "object/ipc_object.h"
#include "object/kernel_object.h"

class ReplyPort;

template <>
struct KernelObjectTag<ReplyPort> {
  static const uint64_t type = KernelObject::REPLY_PORT;
};

class ReplyPort : public IpcObject {
 public:
  uint64_t TypeTag() override { return KernelObject::REPLY_PORT; }
  static glcr::RefPtr<ReplyPort> Create();

  virtual MessageQueue& GetSendMessageQueue() override {
    return message_holder_;
  }
  virtual MessageQueue& GetRecvMessageQueue() override {
    return message_holder_;
  }

 private:
  SingleMessageQueue message_holder_;

  ReplyPort() {}
};
