#pragma once

#include "scheduler/process.h"
#include "scheduler/thread.h"

class Scheduler {
 public:
  // Initializes the scheduler in a disabled state.
  //
  // Requires the process manager to have been initialized.
  static void Init();

  void Enable() { enabled_ = true; }

  Process& CurrentProcess() { return current_thread_->process(); }
  Thread& CurrentThread() { return *current_thread_; }

  void Enqueue(const SharedPtr<Thread> thread) {
    runnable_threads_.PushBack(thread);
  }

  void Preempt();
  void Yield();

 private:
  bool enabled_ = false;

  SharedPtr<Thread> current_thread_;
  LinkedList<SharedPtr<Thread>> runnable_threads_;

  SharedPtr<Thread> sleep_thread_;

  Scheduler();
  void SwapToCurrent(Thread& prev);
};

extern Scheduler* gScheduler;
