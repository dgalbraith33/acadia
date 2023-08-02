#include "lib/message_queue.h"

#include "debug/debug.h"
#include "scheduler/scheduler.h"

z_err_t UnboundedMessageQueue::PushBack(uint64_t num_bytes, const void* bytes,
                                        uint64_t num_caps,
                                        const z_cap_t* caps) {
  if (num_bytes > 0x1000) {
    dbgln("Large message size unimplemented: %x", num_bytes);
    return glcr::UNIMPLEMENTED;
  }

  auto message = glcr::MakeShared<Message>();
  message->num_bytes = num_bytes;
  message->bytes = new uint8_t[num_bytes];
  for (uint64_t i = 0; i < num_bytes; i++) {
    message->bytes[i] = static_cast<const uint8_t*>(bytes)[i];
  }

  for (uint64_t i = 0; i < num_caps; i++) {
    // FIXME: This would feel safer closer to the relevant syscall.
    // FIXME: Race conditions on get->check->release here. Would be better to
    // have that as a single call on the process. (This pattern repeats other
    // places too).
    auto cap = gScheduler->CurrentProcess().GetCapability(caps[i]);
    if (!cap) {
      return glcr::CAP_NOT_FOUND;
    }
    if (!cap->HasPermissions(kZionPerm_Transmit)) {
      return glcr::CAP_PERMISSION_DENIED;
    }
    cap = gScheduler->CurrentProcess().ReleaseCapability(caps[i]);
    message->caps.PushBack(cap);
  }

  MutexHolder h(mutex_);
  pending_messages_.PushBack(message);

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return glcr::OK;
}

z_err_t UnboundedMessageQueue::PopFront(uint64_t* num_bytes, void* bytes,
                                        uint64_t* num_caps, z_cap_t* caps) {
  mutex_.Lock();
  while (pending_messages_.empty()) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }
  mutex_.Unlock();

  MutexHolder lock(mutex_);
  auto next_msg = pending_messages_.PeekFront();
  if (next_msg->num_bytes > *num_bytes) {
    return glcr::BUFFER_SIZE;
  }
  if (next_msg->caps.size() > *num_caps) {
    return glcr::BUFFER_SIZE;
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
  return glcr::OK;
}

void UnboundedMessageQueue::WriteKernel(uint64_t init,
                                        glcr::RefPtr<Capability> cap) {
  // FIXME: Add synchronization here in case it is ever used outside of init.
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

glcr::ErrorCode SingleMessageQueue::PushBack(uint64_t num_bytes,
                                             const void* bytes,
                                             uint64_t num_caps,
                                             const z_cap_t* caps) {
  MutexHolder h(mutex_);
  if (has_written_) {
    return glcr::FAILED_PRECONDITION;
  }
  num_bytes_ = num_bytes;
  bytes_ = new uint8_t[num_bytes];

  for (uint64_t i = 0; i < num_bytes; i++) {
    bytes_[i] = reinterpret_cast<const uint8_t*>(bytes)[i];
  }

  for (uint64_t i = 0; i < num_caps; i++) {
    // FIXME: This would feel safer closer to the relevant syscall.
    auto cap = gScheduler->CurrentProcess().GetCapability(caps[i]);
    if (!cap) {
      return glcr::CAP_NOT_FOUND;
    }
    if (!cap->HasPermissions(kZionPerm_Transmit)) {
      return glcr::CAP_PERMISSION_DENIED;
    }
    cap = gScheduler->CurrentProcess().ReleaseCapability(caps[i]);
    caps_.PushBack(cap);
  }

  has_written_ = true;

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }

  return glcr::OK;
}

glcr::ErrorCode SingleMessageQueue::PopFront(uint64_t* num_bytes, void* bytes,
                                             uint64_t* num_caps,
                                             z_cap_t* caps) {
  mutex_.Lock();
  while (!has_written_) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }
  mutex_.Unlock();

  MutexHolder lock(mutex_);
  if (has_read_) {
    return glcr::FAILED_PRECONDITION;
  }

  if (num_bytes_ > *num_bytes) {
    return glcr::BUFFER_SIZE;
  }
  if (caps_.size() > *num_caps) {
    return glcr::BUFFER_SIZE;
  }

  *num_bytes = num_bytes_;
  for (uint64_t i = 0; i < num_bytes_; i++) {
    reinterpret_cast<uint8_t*>(bytes)[i] = bytes_[i];
  }

  *num_caps = caps_.size();
  auto& proc = gScheduler->CurrentProcess();
  for (uint64_t i = 0; i < *num_caps; i++) {
    caps[i] = proc.AddExistingCapability(caps_.PopFront());
  }
  has_read_ = true;

  return glcr::OK;
}
