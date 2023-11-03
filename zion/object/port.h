#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>

#include "capability/capability.h"
#include "lib/message_queue.h"
#include "object/ipc_object.h"
#include "object/kernel_object.h"
#include "object/thread.h"

class Port;

template <>
struct KernelObjectTag<Port> {
  static const uint64_t type = KernelObject::PORT;
};

class Port : public IpcObject {
 public:
  uint64_t TypeTag() override { return KernelObject::PORT; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Write | kZionPerm_Read | kZionPerm_Duplicate |
           kZionPerm_Transmit;
  }

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
