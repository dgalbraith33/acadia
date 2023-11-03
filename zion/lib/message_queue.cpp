#include "lib/message_queue.h"

#include "debug/debug.h"
#include "scheduler/scheduler.h"

glcr::ErrorCode UnboundedMessageQueue::PushBack(
    const glcr::ArrayView<uint8_t>& message,
    const glcr::ArrayView<z_cap_t>& caps, z_cap_t reply_cap) {
  if (message.size() > 0x1000) {
    dbgln("Large message size unimplemented: %x", message.size());
    return glcr::UNIMPLEMENTED;
  }

  auto msg_struct = glcr::MakeShared<Message>();
  msg_struct->message = glcr::Array<uint8_t>(message);

  if (reply_cap != kZionInvalidCapability) {
    // FIXME: We're just trusting that capability has the correct permissions.
    msg_struct->reply_cap =
        gScheduler->CurrentProcess().ReleaseCapability(reply_cap);
  }

  for (uint64_t i = 0; i < caps.size(); i++) {
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
    msg_struct->caps.PushBack(cap);
  }

  MutexHolder h(mutex_);
  pending_messages_.PushBack(msg_struct);

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return glcr::OK;
}

glcr::ErrorCode UnboundedMessageQueue::PopFront(uint64_t* num_bytes,
                                                void* bytes, uint64_t* num_caps,
                                                z_cap_t* caps,
                                                z_cap_t* reply_cap) {
  mutex_->Lock();
  while (pending_messages_.empty()) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    mutex_->Release();
    gScheduler->Yield();
    mutex_->Lock();
  }
  mutex_->Release();

  MutexHolder lock(mutex_);
  auto next_msg = pending_messages_.PeekFront();
  if (next_msg->message.size() > *num_bytes) {
    return glcr::BUFFER_SIZE;
  }
  if (next_msg->caps.size() > *num_caps) {
    return glcr::BUFFER_SIZE;
  }

  next_msg = pending_messages_.PopFront();

  *num_bytes = next_msg->message.size();

  for (uint64_t i = 0; i < *num_bytes; i++) {
    static_cast<uint8_t*>(bytes)[i] = next_msg->message[i];
  }

  auto& proc = gScheduler->CurrentProcess();
  if (reply_cap != nullptr) {
    if (!next_msg->reply_cap) {
      dbgln("Tried to read reply capability off of a message without one");
      return glcr::INTERNAL;
    }
    *reply_cap = proc.AddExistingCapability(next_msg->reply_cap);
  }

  *num_caps = next_msg->caps.size();
  for (uint64_t i = 0; i < *num_caps; i++) {
    caps[i] = proc.AddExistingCapability(next_msg->caps.PopFront());
  }
  return glcr::OK;
}

void UnboundedMessageQueue::WriteKernel(uint64_t init,
                                        glcr::RefPtr<Capability> cap) {
  // FIXME: Add synchronization here in case it is ever used outside of init.
  auto msg = glcr::MakeShared<Message>();
  msg->message = glcr::Array<uint8_t>(sizeof(init));

  uint8_t* data = reinterpret_cast<uint8_t*>(&init);
  for (uint8_t i = 0; i < sizeof(init); i++) {
    msg->message[i] = data[i];
  }
  msg->caps.PushBack(cap);

  pending_messages_.PushBack(msg);
}

glcr::ErrorCode SingleMessageQueue::PushBack(
    const glcr::ArrayView<uint8_t>& message,
    const glcr::ArrayView<z_cap_t>& caps, z_cap_t reply_port) {
  MutexHolder h(mutex_);
  if (has_written_) {
    return glcr::FAILED_PRECONDITION;
  }
  message_ = message;

  if (reply_port != kZionInvalidCapability) {
    dbgln("Sent a reply port to a single message queue");
    return glcr::INTERNAL;
  }

  for (uint64_t i = 0; i < caps.size(); i++) {
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
                                             uint64_t* num_caps, z_cap_t* caps,
                                             z_cap_t* reply_port) {
  mutex_->Lock();
  while (!has_written_) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    mutex_->Release();
    gScheduler->Yield();
    mutex_->Lock();
  }
  mutex_->Release();

  MutexHolder lock(mutex_);
  if (has_read_) {
    return glcr::FAILED_PRECONDITION;
  }

  if (message_.size() > *num_bytes) {
    return glcr::BUFFER_SIZE;
  }
  if (caps_.size() > *num_caps) {
    return glcr::BUFFER_SIZE;
  }

  *num_bytes = message_.size();
  for (uint64_t i = 0; i < message_.size(); i++) {
    reinterpret_cast<uint8_t*>(bytes)[i] = message_[i];
  }

  if (reply_port != nullptr) {
    dbgln("Tried to read a reply port a single message queue");
    return glcr::INTERNAL;
  }

  *num_caps = caps_.size();
  auto& proc = gScheduler->CurrentProcess();
  for (uint64_t i = 0; i < *num_caps; i++) {
    caps[i] = proc.AddExistingCapability(caps_.PopFront());
  }
  has_read_ = true;

  return glcr::OK;
}
