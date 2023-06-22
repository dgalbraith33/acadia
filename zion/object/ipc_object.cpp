#include "object/ipc_object.h"

#include "scheduler/scheduler.h"

glcr::ErrorCode IpcObject::Send(uint64_t num_bytes, const void* bytes,
                                uint64_t num_caps, const z_cap_t* caps) {
  auto& message_queue = GetSendMessageQueue();
  return message_queue.PushBack(num_bytes, bytes, num_caps, caps);
}

glcr::ErrorCode IpcObject::Recv(uint64_t* num_bytes, void* bytes,
                                uint64_t* num_caps, z_cap_t* caps) {
  auto& message_queue = GetRecvMessageQueue();
  return message_queue.PopFront(num_bytes, bytes, num_caps, caps);
}
