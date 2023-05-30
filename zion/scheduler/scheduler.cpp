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

  void Yield() {
    if (!enabled_) {
      return;
    }
    asm volatile("cli");

    SharedPtr<Thread> prev = current_thread_;
    SharedPtr<Thread> next;
    if (prev == sleep_thread_) {
      if (runnable_threads_.size() == 0) {
        // Continue sleeping.
        return;
      } else {
        // FIXME: Memory operation.
        next = runnable_threads_.PopFront();
        prev->SetState(Thread::RUNNABLE);
      }
    } else {
      // Normal thread running.
      if (prev->GetState() == Thread::RUNNING) {
        if (runnable_threads_.size() == 0) {
          // This thread can continue.
          return;
        }
        prev->SetState(Thread::RUNNABLE);
        next = runnable_threads_.CycleFront(prev);
      } else {
        // Thread blocked/exited.
        if (runnable_threads_.size() == 0) {
          next = sleep_thread_;
          dbgln("Sleeping");
          DumpProcessStates(proc_list_);
        } else {
          // FIXME: Memory operation.
          next = runnable_threads_.PopFront();
        }
      }
    }

    if (next->GetState() != Thread::RUNNABLE) {
      panic("Non-runnable thread in the queue");
    }

    next->SetState(Thread::RUNNING);
    current_thread_ = next;

    context_switch(prev->Rsp0Ptr(), next->Rsp0Ptr());

    asm volatile("sti");
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

void Yield() { GetScheduler().Yield(); }

void InsertProcess(Process* process) { GetScheduler().InsertProcess(process); }
void EnqueueThread(Thread* thread) { GetScheduler().Enqueue(thread); }

Process& CurrentProcess() { return GetScheduler().CurrentProcess(); }
Thread& CurrentThread() { return GetScheduler().CurrentThread(); }

}  // namespace sched
