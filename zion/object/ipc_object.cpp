#include "object/ipc_object.h"

#include "scheduler/scheduler.h"

glcr::ErrorCode IpcObject::Send(IpcMessage&& message) {
  return GetSendMessageQueue().PushBack(glcr::Move(message));
}

glcr::ErrorOr<IpcMessage> IpcObject::Recv(uint64_t data_buf_size,
                                          uint64_t cap_buf_size) {
  return GetRecvMessageQueue().PopFront(data_buf_size, cap_buf_size);
}
