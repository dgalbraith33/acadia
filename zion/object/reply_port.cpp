#include "object/reply_port.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<ReplyPort> ReplyPort::Create() {
  return glcr::AdoptPtr(new ReplyPort);
}

uint64_t ReplyPort::Write(uint64_t num_bytes, const void* data,
                          uint64_t num_caps, uint64_t* caps) {
  MutexHolder h(mutex_);
  RET_ERR(message_holder_.PushBack(num_bytes, data, num_caps, caps));

  if (blocked_thread_) {
    // FIXME: We need to handle the case where the blocked thread has died I
    // think.
    blocked_thread_->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(blocked_thread_);
    blocked_thread_ = nullptr;
  }
  return glcr::OK;
}

uint64_t ReplyPort::Read(uint64_t* num_bytes, void* data, uint64_t* num_caps,
                         uint64_t* caps) {
  mutex_.Lock();
  if (message_holder_.empty()) {
    // Multiple threads can't block on a reply port.
    if (blocked_thread_) {
      mutex_.Unlock();
      return glcr::FAILED_PRECONDITION;
    }
    blocked_thread_ = gScheduler->CurrentThread();
    blocked_thread_->SetState(Thread::BLOCKED);
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }

  return message_holder_.PopFront(num_bytes, data, num_caps, caps);
}
