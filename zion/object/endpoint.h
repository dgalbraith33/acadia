#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/status/error.h>

#include "lib/message_queue.h"
#include "lib/mutex.h"
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
  static glcr::RefPtr<Endpoint> Create();

  glcr::ErrorCode Read(uint64_t* num_bytes, void* data,
                       z_cap_t* reply_port_cap);

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
