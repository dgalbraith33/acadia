#include "scheduler/scheduler.h"

#include "common/gdt.h"
#include "debug/debug.h"
#include "lib/linked_list.h"
#include "scheduler/process_manager.h"

namespace {

extern "C" void context_switch(uint64_t* current_esp, uint64_t* next_esp);

}  // namespace

Scheduler* gScheduler = nullptr;

void Scheduler::Init() { gScheduler = new Scheduler(); }
Scheduler::Scheduler() {
  Process& root = gProcMan->FromId(0);
  sleep_thread_ = root.GetThread(0);
  // TODO: Implement a separate sleep thread?
  current_thread_ = sleep_thread_;
}

void Scheduler::SwapToCurrent(Thread& prev) {
  if (current_thread_->GetState() != Thread::RUNNABLE) {
    panic("Swapping to non-runnable thread.");
  }
  current_thread_->SetState(Thread::RUNNING);

  SetRsp0(current_thread_->Rsp0Start());
  context_switch(prev.Rsp0Ptr(), current_thread_->Rsp0Ptr());

  asm volatile("sti");
}

void Scheduler::Preempt() {
  if (!enabled_) {
    return;
  }

  asm volatile("cli");
  if (current_thread_ == sleep_thread_) {
    // Sleep should never be preempted. (We should yield it if another thread
    // becomes scheduleable).
    // FIXME: We should yield these threads instead of preempting them.
    if (runnable_threads_.size() > 0) {
      current_thread_ = runnable_threads_.PopFront();
      sleep_thread_->SetState(Thread::RUNNABLE);
      SwapToCurrent(*sleep_thread_);
    } else {
      asm volatile("sti");
    }
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
    dbgln("WARN Scheduler skipped yield.");
    return;
  }
  asm volatile("cli");

  glcr::RefPtr<Thread> prev = current_thread_;
  if (prev == sleep_thread_) {
    if (runnable_threads_.size() == 0) {
      panic("Sleep thread yielded without next.");
      return;
    } else {
      current_thread_ = runnable_threads_.PopFront();
      prev->SetState(Thread::RUNNABLE);
    }
  } else {
    if (runnable_threads_.size() == 0) {
      current_thread_ = sleep_thread_;
      dbgln("Sleeping");
      gProcMan->DumpProcessStates();
    } else {
      current_thread_ = runnable_threads_.PopFront();
    }
  }

  SwapToCurrent(*prev);
}
