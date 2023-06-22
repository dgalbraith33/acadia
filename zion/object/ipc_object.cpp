#include "object/ipc_object.h"

#include "scheduler/scheduler.h"

glcr::ErrorCode IpcObject::Send(uint64_t num_bytes, const void* bytes,
                                uint64_t num_caps, const z_cap_t* caps) {
  auto& message_queue = GetSendMessageQueue();
  MutexHolder lock(mutex_);
  RET_ERR(message_queue.PushBack(num_bytes, bytes, num_caps, caps));

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return glcr::OK;
}

glcr::ErrorCode IpcObject::Recv(uint64_t* num_bytes, void* bytes,
                                uint64_t* num_caps, z_cap_t* caps) {
  auto& message_queue = GetRecvMessageQueue();
  mutex_.Lock();
  while (message_queue.empty()) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    mutex_.Unlock();
    gScheduler->Yield();
    mutex_.Lock();
  }
  mutex_.Unlock();

  MutexHolder lock(mutex_);
  return message_queue.PopFront(num_bytes, bytes, num_caps, caps);
}
