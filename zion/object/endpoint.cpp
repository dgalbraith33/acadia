#include "object/endpoint.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<Endpoint> Endpoint::Create() {
  return glcr::AdoptPtr(new Endpoint);
}

glcr::ErrorCode Endpoint::Write(uint64_t num_bytes, const void* data,
                                z_cap_t reply_port_cap) {
  MutexHolder h(mutex_);
  RET_ERR(message_queue_.PushBack(num_bytes, data, 1, &reply_port_cap));

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }

  return glcr::OK;
}
glcr::ErrorCode Endpoint::Read(uint64_t* num_bytes, void* data,
                               z_cap_t* reply_port_cap) {
  mutex_.Lock();
  while (message_queue_.empty()) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }
  mutex_.Unlock();

  MutexHolder h(mutex_);

  uint64_t num_caps = 1;
  RET_ERR(message_queue_.PopFront(num_bytes, data, &num_caps, reply_port_cap));

  if (num_caps != 1) {
    return glcr::INTERNAL;
  }
  return glcr::OK;
}
