#include "object/endpoint.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<Endpoint> Endpoint::Create() {
  return glcr::AdoptPtr(new Endpoint);
}

glcr::ErrorCode Endpoint::Send(uint64_t num_bytes, const void* data,
                               uint64_t num_caps, const z_cap_t* caps,
                               z_cap_t reply_port_cap) {
  auto& message_queue = GetSendMessageQueue();
  return message_queue.PushBack(num_bytes, data, num_caps, caps,
                                reply_port_cap);
}
glcr::ErrorCode Endpoint::Recv(uint64_t* num_bytes, void* data,
                               uint64_t* num_caps, z_cap_t* caps,
                               z_cap_t* reply_port_cap) {
  auto& message_queue = GetRecvMessageQueue();
  return message_queue.PopFront(num_bytes, data, num_caps, caps,
                                reply_port_cap);
}
