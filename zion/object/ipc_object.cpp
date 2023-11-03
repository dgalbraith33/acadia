#include "object/ipc_object.h"

#include "scheduler/scheduler.h"

glcr::ErrorCode IpcObject::Send(const glcr::ArrayView<uint8_t>& message,
                                const glcr::ArrayView<z_cap_t>& caps) {
  return Send(message, caps, kZionInvalidCapability);
}

glcr::ErrorCode IpcObject::Send(const glcr::ArrayView<uint8_t>& message,
                                const glcr::ArrayView<z_cap_t>& caps,
                                const z_cap_t reply_port) {
  auto& message_queue = GetSendMessageQueue();
  return message_queue.PushBack(message, caps, reply_port);
}

glcr::ErrorOr<IpcMessage> IpcObject::Recv(uint64_t data_buf_size,
                                          uint64_t cap_buf_size) {
  return GetRecvMessageQueue().PopFront(data_buf_size, cap_buf_size);
}
