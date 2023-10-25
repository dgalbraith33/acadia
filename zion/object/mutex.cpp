#include "object/mutex.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<Mutex> Mutex::Create() {
  return glcr::AdoptPtr<Mutex>(new Mutex());
}

// FIXME: We almost certainly have some race conditions
// between this and unlock where we could end up with
// a thread in the blocked_threads_ queue while noone is holding the lock.
void Mutex::Lock() {
  while (__atomic_fetch_or(&lock_, 0x1, __ATOMIC_SEQ_CST) == 0x1) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    gScheduler->Yield();
  }
}

// FIXME: Check that this thread is the one that is holding the mutex before
// releasing it.
void Mutex::Release() {
  lock_ = 0;
  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
}
