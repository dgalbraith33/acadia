#include "scheduler/scheduler.h"

#include "debug/debug.h"
#include "lib/linked_list.h"

namespace sched {
namespace {

extern "C" void context_switch(uint64_t* current_esp, uint64_t* next_esp);

void DumpProcessStates(LinkedList<SharedPtr<Process>>& proc_list) {
  dbgln("Process States: %u", proc_list.size());
  auto iter = proc_list.begin();
  while (iter != proc_list.end()) {
    dbgln("%u: %u", iter->id(), iter->GetState());
    ++iter;
  }
}

class Scheduler {
 public:
  Scheduler() {
    SharedPtr<Process> root = Process::RootProcess();
    sleep_thread_ = root->GetThread(0);
    // TODO: Implement a separate sleep thread?
    current_thread_ = sleep_thread_;
    proc_list_.PushBack(root);
  }
  void Enable() { enabled_ = true; }

  Process& CurrentProcess() { return CurrentThread().process(); }
  Thread& CurrentThread() { return *current_thread_; }

  void InsertProcess(Process* process) { proc_list_.PushBack(process); }
  void Enqueue(Thread* thread) { runnable_threads_.PushBack(thread); }

  void SwapToCurrent(Thread& prev) {
    if (current_thread_->GetState() != Thread::RUNNABLE) {
      panic("Swapping to non-runnable thread.");
    }
    current_thread_->SetState(Thread::RUNNING);

    context_switch(prev.Rsp0Ptr(), current_thread_->Rsp0Ptr());

    asm volatile("sti");
  }

  void Preempt() {
    if (!enabled_) {
      return;
    }

    asm volatile("cli");
    if (current_thread_ == sleep_thread_) {
      // Sleep should never be preempted. (We should yield it if another thread
      // becomes scheduleable).
      return;
    }

    if (runnable_threads_.size() == 0) {
      // Continue.
      return;
    }

    SharedPtr<Thread> prev = current_thread_;
    prev->SetState(Thread::RUNNABLE);
    current_thread_ = runnable_threads_.CycleFront(prev);

    SwapToCurrent(*prev);
  }

  void Yield() {
    if (!enabled_) {
      dbgln("WARN Scheduler skipped yield.");
      return;
    }
    asm volatile("cli");

    SharedPtr<Thread> prev = current_thread_;
    if (prev == sleep_thread_) {
      if (runnable_threads_.size() == 0) {
        panic("Sleep thread yielded without next.");
        return;
      } else {
        // FIXME: Memory operation.
        current_thread_ = runnable_threads_.PopFront();
        prev->SetState(Thread::RUNNABLE);
      }
    } else {
      if (runnable_threads_.size() == 0) {
        current_thread_ = sleep_thread_;
        dbgln("Sleeping");
        DumpProcessStates(proc_list_);
      } else {
        // FIXME: Memory operation.
        current_thread_ = runnable_threads_.PopFront();
      }
    }

    SwapToCurrent(*prev);
  }

 private:
  bool enabled_ = false;
  // TODO: move this to a separate process manager class.
  LinkedList<SharedPtr<Process>> proc_list_;

  SharedPtr<Thread> current_thread_;
  LinkedList<SharedPtr<Thread>> runnable_threads_;

  SharedPtr<Thread> sleep_thread_;
};

static Scheduler* gScheduler = nullptr;

Scheduler& GetScheduler() {
  if (!gScheduler) {
    panic("Scheduler not initialized");
  }
  return *gScheduler;
}

}  // namespace

void InitScheduler() { gScheduler = new Scheduler(); }
void EnableScheduler() { GetScheduler().Enable(); }

void Preempt() { GetScheduler().Preempt(); }
void Yield() { GetScheduler().Yield(); }

void InsertProcess(Process* process) { GetScheduler().InsertProcess(process); }
void EnqueueThread(Thread* thread) { GetScheduler().Enqueue(thread); }

Process& CurrentProcess() { return GetScheduler().CurrentProcess(); }
Thread& CurrentThread() { return GetScheduler().CurrentThread(); }

}  // namespace sched
