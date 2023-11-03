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

  IpcMessage msg_struct;
  msg_struct.data = glcr::Array<uint8_t>(message);

  if (reply_cap != kZionInvalidCapability) {
    // FIXME: We're just trusting that capability has the correct permissions.
    msg_struct.reply_cap =
        gScheduler->CurrentProcess().ReleaseCapability(reply_cap);
  }

  msg_struct.caps.Resize(caps.size());
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
    msg_struct.caps.PushBack(cap);
  }

  MutexHolder h(mutex_);
  pending_messages_.PushBack(glcr::Move(msg_struct));

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return glcr::OK;
}

glcr::ErrorOr<IpcMessage> UnboundedMessageQueue::PopFront(
    uint64_t data_buf_size, uint64_t cap_buf_size) {
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
  auto& next_msg = pending_messages_.PeekFront();
  if (next_msg.data.size() > data_buf_size) {
    return glcr::BUFFER_SIZE;
  }
  if (next_msg.caps.size() > cap_buf_size) {
    return glcr::BUFFER_SIZE;
  }

  return pending_messages_.PopFront();
}

void UnboundedMessageQueue::WriteKernel(uint64_t init,
                                        glcr::RefPtr<Capability> cap) {
  // FIXME: Add synchronization here in case it is ever used outside of init.
  IpcMessage msg;
  msg.data = glcr::Array<uint8_t>(sizeof(init));

  uint8_t* data = reinterpret_cast<uint8_t*>(&init);
  for (uint8_t i = 0; i < sizeof(init); i++) {
    msg.data[i] = data[i];
  }
  msg.caps.PushBack(cap);

  pending_messages_.PushBack(glcr::Move(msg));
}

glcr::ErrorCode SingleMessageQueue::PushBack(
    const glcr::ArrayView<uint8_t>& message,
    const glcr::ArrayView<z_cap_t>& caps, z_cap_t reply_port) {
  MutexHolder h(mutex_);
  if (has_written_) {
    return glcr::FAILED_PRECONDITION;
  }
  message_.data = message;

  if (reply_port != kZionInvalidCapability) {
    dbgln("Sent a reply port to a single message queue");
    return glcr::INTERNAL;
  }

  message_.caps.Resize(caps.size());
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
    message_.caps.PushBack(cap);
  }

  has_written_ = true;

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }

  return glcr::OK;
}

glcr::ErrorOr<IpcMessage> SingleMessageQueue::PopFront(uint64_t data_buf_size,
                                                       uint64_t cap_buf_size) {
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

  if (message_.data.size() > data_buf_size) {
    return glcr::BUFFER_SIZE;
  }
  if (message_.caps.size() > cap_buf_size) {
    return glcr::BUFFER_SIZE;
  }

  has_read_ = true;

  return glcr::Move(message_);
}
