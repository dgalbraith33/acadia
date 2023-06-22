#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>

#include "capability/capability.h"
#include "lib/message_queue.h"
#include "lib/mutex.h"
#include "object/ipc_object.h"
#include "object/kernel_object.h"
#include "object/thread.h"
#include "usr/zcall_internal.h"

class Port;

template <>
struct KernelObjectTag<Port> {
  static const uint64_t type = KernelObject::PORT;
};

class Port : public IpcObject {
 public:
  uint64_t TypeTag() override { return KernelObject::PORT; }

  Port() = default;

  void WriteKernel(uint64_t init, glcr::RefPtr<Capability> cap);

  virtual MessageQueue& GetSendMessageQueue() override {
    return message_queue_;
  }
  virtual MessageQueue& GetRecvMessageQueue() override {
    return message_queue_;
  }

 private:
  UnboundedMessageQueue message_queue_;
};
