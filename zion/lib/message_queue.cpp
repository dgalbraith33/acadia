#include "lib/message_queue.h"

#include "scheduler/scheduler.h"

z_err_t UnboundedMessageQueue::PushBack(uint64_t num_bytes, const void* bytes,
                                        uint64_t num_caps,
                                        const z_cap_t* caps) {
  if (num_bytes > 0x1000) {
    dbgln("Large message size unimplemented: %x", num_bytes);
    return Z_ERR_UNIMPLEMENTED;
  }

  auto message = glcr::MakeShared<Message>();
  message->num_bytes = num_bytes;
  message->bytes = new uint8_t[num_bytes];
  for (uint64_t i = 0; i < num_bytes; i++) {
    message->bytes[i] = static_cast<const uint8_t*>(bytes)[i];
  }

  for (uint64_t i = 0; i < num_caps; i++) {
    // FIXME: This would feel safer closer to the relevant syscall.
    auto cap = gScheduler->CurrentProcess().ReleaseCapability(caps[i]);
    if (!cap) {
      return Z_ERR_CAP_NOT_FOUND;
    }
    message->caps.PushBack(cap);
  }

  pending_messages_.PushBack(message);
  return Z_OK;
}

z_err_t UnboundedMessageQueue::PopFront(uint64_t* num_bytes, void* bytes,
                                        uint64_t* num_caps, z_cap_t* caps) {
  auto next_msg = pending_messages_.PeekFront();
  if (next_msg->num_bytes > *num_bytes) {
    return Z_ERR_BUFF_SIZE;
  }
  if (next_msg->caps.size() > *num_caps) {
    return Z_ERR_BUFF_SIZE;
  }

  next_msg = pending_messages_.PopFront();

  *num_bytes = next_msg->num_bytes;

  for (uint64_t i = 0; i < *num_bytes; i++) {
    static_cast<uint8_t*>(bytes)[i] = next_msg->bytes[i];
  }

  *num_caps = next_msg->caps.size();
  auto& proc = gScheduler->CurrentProcess();
  for (uint64_t i = 0; i < *num_caps; i++) {
    caps[i] = proc.AddExistingCapability(next_msg->caps.PopFront());
  }
  return Z_OK;
}

void UnboundedMessageQueue::WriteKernel(uint64_t init,
                                        glcr::RefPtr<Capability> cap) {
  auto msg = glcr::MakeShared<Message>();
  msg->bytes = new uint8_t[8];
  msg->num_bytes = sizeof(init);

  uint8_t* data = reinterpret_cast<uint8_t*>(&init);
  for (uint8_t i = 0; i < sizeof(init); i++) {
    msg->bytes[i] = data[i];
  }
  msg->caps.PushBack(cap);

  pending_messages_.PushBack(msg);
}
