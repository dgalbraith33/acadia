#include "object/semaphore.h"

#include "scheduler/scheduler.h"

glcr::RefPtr<Semaphore> Semaphore::Create() {
  return glcr::AdoptPtr<Semaphore>(new Semaphore());
}

// FIXME: We almost certainly have some race conditions
// between this and unlock where we could end up with
// a thread in the blocked_threads_ queue while noone is holding the lock.
void Semaphore::Signal() {
  __atomic_fetch_add(&lock_, 0x1, __ATOMIC_SEQ_CST);
  if (blocked_threads_.size() > 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
}

void Semaphore::Wait() {
  while (lock_ == 0) {
    auto thread = gScheduler->CurrentThread();
    thread->SetState(Thread::BLOCKED);
    blocked_threads_.PushBack(thread);
    gScheduler->Yield();
  }
  __atomic_fetch_sub(&lock_, 0x1, __ATOMIC_SEQ_CST);
}
