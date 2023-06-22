#pragma once

#include <glacier/status/error.h>

#include "include/ztypes.h"
#include "lib/message_queue.h"
#include "object/kernel_object.h"

class IpcObject : public KernelObject {
 public:
  IpcObject(){};
  virtual ~IpcObject() {}

  virtual glcr::ErrorCode Send(uint64_t num_bytes, const void* bytes,
                               uint64_t num_caps, const z_cap_t* caps) final;
  virtual glcr::ErrorCode Recv(uint64_t* num_bytes, void* bytes,
                               uint64_t* num_caps, z_cap_t* caps) final;

  bool HasMessages() { return !GetRecvMessageQueue().empty(); }

  virtual MessageQueue& GetSendMessageQueue() = 0;
  virtual MessageQueue& GetRecvMessageQueue() = 0;
};
