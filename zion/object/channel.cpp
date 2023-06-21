#include "object/channel.h"

#include "include/ztypes.h"
#include "scheduler/scheduler.h"

Pair<glcr::RefPtr<Channel>, glcr::RefPtr<Channel>>
Channel::CreateChannelPair() {
  auto c1 = glcr::MakeRefCounted<Channel>();
  auto c2 = glcr::MakeRefCounted<Channel>();
  c1->SetPeer(c2);
  c2->SetPeer(c1);
  return {c1, c2};
}

z_err_t Channel::Write(uint64_t num_bytes, const void* bytes, uint64_t num_caps,
                       const z_cap_t* caps) {
  return peer_->WriteInternal(num_bytes, bytes, num_caps, caps);
}

z_err_t Channel::Read(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
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

z_err_t Channel::WriteInternal(uint64_t num_bytes, const void* bytes,
                               uint64_t num_caps, const z_cap_t* caps) {
  MutexHolder lock(mutex_);
  RET_ERR(message_queue_.PushBack(num_bytes, bytes, num_caps, caps));

  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  return Z_OK;
}
