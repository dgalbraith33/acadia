#include "lib/message_queue.h"

#include "debug/debug.h"
#include "scheduler/scheduler.h"

glcr::ErrorCode UnboundedMessageQueue::PushBack(IpcMessage&& message) {
  MutexHolder h(mutex_);
  pending_messages_.PushBack(glcr::Move(message));

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

glcr::ErrorCode SingleMessageQueue::PushBack(IpcMessage&& message) {
  if (message.reply_cap) {
    dbgln("Sent a reply port to a single message queue");
    return glcr::INTERNAL;
  }

  MutexHolder h(mutex_);
  if (has_written_) {
    dbgln("Double write to reply port.");
    return glcr::FAILED_PRECONDITION;
  }

  message_ = glcr::Move(message);
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
    dbgln("Double read from reply port.");
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
