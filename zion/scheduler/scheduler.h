#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>

#include "object/process.h"
#include "object/thread.h"

class Scheduler {
 public:
  // Initializes the scheduler in a disabled state.
  //
  // Requires the process manager to have been initialized.
  static void Init();

  void Enable() { enabled_ = true; }

  Process& CurrentProcess() { return current_thread_->process(); }
  glcr::RefPtr<Thread> CurrentThread() { return current_thread_; }

  void Enqueue(const glcr::RefPtr<Thread>& thread);

  void Preempt();
  void Yield();

  void Sleep(uint64_t millis);

 private:
  bool enabled_ = false;

  glcr::RefPtr<Thread> current_thread_;
  glcr::IntrusiveList<Thread> runnable_threads_;

  glcr::IntrusiveList<Thread> sleeping_threads_;

  glcr::RefPtr<Thread> sleep_thread_;

  Scheduler();
  void SwapToCurrent(Thread& prev);

  void ClearDeadThreadsFromFront();
  void DecrementSleepingThreads();
};

extern Scheduler* gScheduler;
