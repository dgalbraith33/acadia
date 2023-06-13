#pragma once

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
  RefPtr<Thread> CurrentThread() { return current_thread_; }

  void Enqueue(const RefPtr<Thread>& thread) {
    runnable_threads_.PushBack(thread);
  }

  void Preempt();
  void Yield();

 private:
  bool enabled_ = false;

  RefPtr<Thread> current_thread_;
  LinkedList<RefPtr<Thread>> runnable_threads_;

  RefPtr<Thread> sleep_thread_;

  Scheduler();
  void SwapToCurrent(Thread& prev);
};

extern Scheduler* gScheduler;
