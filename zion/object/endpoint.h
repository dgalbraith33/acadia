#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/status/error.h>

#include "lib/message_queue.h"
#include "object/ipc_object.h"
#include "object/kernel_object.h"

class Endpoint;
class ReplyPort;

template <>
struct KernelObjectTag<Endpoint> {
  static const uint64_t type = KernelObject::ENDPOINT;
};

class Endpoint : public IpcObject {
 public:
  uint64_t TypeTag() override { return KernelObject::ENDPOINT; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Read | kZionPerm_Write | kZionPerm_Duplicate |
           kZionPerm_Transmit;
  }

  static glcr::RefPtr<Endpoint> Create();

  virtual MessageQueue& GetSendMessageQueue() override {
    return message_queue_;
  }
  virtual MessageQueue& GetRecvMessageQueue() override {
    return message_queue_;
  }

 private:
  UnboundedMessageQueue message_queue_;

  Endpoint() {}
};
