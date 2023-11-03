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

glcr::ErrorCode IpcObject::Recv(uint64_t* num_bytes, void* bytes,
                                uint64_t* num_caps, z_cap_t* caps) {
  return Recv(num_bytes, bytes, num_caps, caps, nullptr);
}

glcr::ErrorCode IpcObject::Recv(uint64_t* num_bytes, void* bytes,
                                uint64_t* num_caps, z_cap_t* caps,
                                z_cap_t* reply_port) {
  auto& message_queue = GetRecvMessageQueue();
  return message_queue.PopFront(num_bytes, bytes, num_caps, caps, reply_port);
}
