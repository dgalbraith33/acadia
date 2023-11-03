#pragma once

#include <glacier/status/error.h>

#include "include/ztypes.h"
#include "lib/message_queue.h"
#include "object/kernel_object.h"

class IpcObject : public KernelObject {
 public:
  IpcObject(){};
  virtual ~IpcObject() {}

  virtual glcr::ErrorCode Send(IpcMessage&& message) final;

  virtual glcr::ErrorOr<IpcMessage> Recv(uint64_t data_buf_size,
                                         uint64_t cap_buf_size) final;

  bool HasMessages() { return !GetRecvMessageQueue().empty(); }

  virtual MessageQueue& GetSendMessageQueue() = 0;
  virtual MessageQueue& GetRecvMessageQueue() = 0;
};
