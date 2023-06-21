#include "object/port.h"

#include "scheduler/scheduler.h"

Port::Port() {}

z_err_t Port::Write(uint64_t num_bytes, const void* bytes, uint64_t num_caps,
                    const z_cap_t* caps) {
  MutexHolder h(mutex_);
  RET_ERR(message_queue_.PushBack(num_bytes, bytes, num_caps, caps));
  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return Z_OK;
}

z_err_t Port::Read(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
                   z_cap_t* caps) {
  mutex_.Lock();
  while (message_queue_.empty()) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }
  mutex_.Unlock();

  MutexHolder lock(mutex_);
  return message_queue_.PopFront(num_bytes, bytes, num_caps, caps);
}

void Port::WriteKernel(uint64_t init, glcr::RefPtr<Capability> cap) {
  MutexHolder h(mutex_);
  message_queue_.WriteKernel(init, cap);
}

bool Port::HasMessages() {
  MutexHolder h(mutex_);
  return !message_queue_.empty();
}
