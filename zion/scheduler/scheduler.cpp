#include "scheduler/scheduler.h"

#include "common/gdt.h"
#include "debug/debug.h"
#include "scheduler/process_manager.h"

namespace {

extern "C" void context_switch(uint64_t* current_esp, uint64_t* next_esp,
                               uint8_t* current_fx_data, uint8_t* next_fx_data);

}  // namespace

Scheduler* gScheduler = nullptr;

void Scheduler::Init() { gScheduler = new Scheduler(); }
Scheduler::Scheduler() {
  Process& root = gProcMan->FromId(0);
  sleep_thread_ = root.GetThread(0);
  current_thread_ = sleep_thread_;
}

void Scheduler::SwapToCurrent(Thread& prev) {
  if (current_thread_->GetState() != Thread::RUNNABLE) {
    panic("Swapping to non-runnable thread.");
  }
  current_thread_->SetState(Thread::RUNNING);

  SetRsp0(current_thread_->Rsp0Start());
  context_switch(prev.Rsp0Ptr(), current_thread_->Rsp0Ptr(), prev.FxData(),
                 current_thread_->FxData());

  asm volatile("sti");
}

void Scheduler::Enqueue(const glcr::RefPtr<Thread>& thread) {
  runnable_threads_.PushBack(thread);
  if (current_thread_ == sleep_thread_) {
    Yield();
  }
}

void Scheduler::Preempt() {
  if (!enabled_) {
    return;
  }

  ClearDeadThreadsFromFront();

  asm volatile("cli");
  if (current_thread_ == sleep_thread_) {
    // Sleep should never be preempted. (We should yield it if another thread
    // becomes scheduleable).
    return;
  }

  if (runnable_threads_.size() == 0) {
    // Continue.
    asm volatile("sti");
    return;
  }

  glcr::RefPtr<Thread> prev = current_thread_;
  prev->SetState(Thread::RUNNABLE);
  runnable_threads_.PushBack(prev);
  current_thread_ = runnable_threads_.PopFront();

  SwapToCurrent(*prev);
}

void Scheduler::Yield() {
  if (!enabled_) {
    // This is expected to fire once at the start when we enqueue the first
    // thread before the scheduler is enabled. Maybe we should get rid of it?
    dbgln("WARN Scheduler skipped yield.");
    return;
  }

  ClearDeadThreadsFromFront();

  asm volatile("cli");

  glcr::RefPtr<Thread> prev = current_thread_;
  if (prev == sleep_thread_) {
    if (runnable_threads_.size() == 0) {
      panic("Sleep thread yielded without next.");
      return;
    } else {
      current_thread_ = runnable_threads_.PopFront();
      if (!prev->IsDying()) {
        prev->SetState(Thread::RUNNABLE);
      }
    }
  } else {
    if (runnable_threads_.size() == 0) {
      current_thread_ = sleep_thread_;
    } else {
      current_thread_ = runnable_threads_.PopFront();
    }
  }

  SwapToCurrent(*prev);
}

void Scheduler::ClearDeadThreadsFromFront() {
  while (runnable_threads_.size() > 0 &&
         runnable_threads_.PeekFront()->IsDying()) {
    runnable_threads_.PopFront();
  }
}
