#include "scheduler/scheduler.h"

#include "debug/debug.h"
#include "lib/linked_list.h"

namespace sched {
namespace {

extern "C" void context_switch(uint64_t* current_esp, uint64_t* next_esp);

// Simple linked list class with the intent of eventually replacing this with a
// map.
class ProcList {
 public:
  ProcList() {}

  // Takes ownership.
  void InsertProcess(Process* proc) {
    if (front_ == nullptr) {
      front_ = new ProcEntry{
          .proc = proc,
          .next = nullptr,
      };
      return;
    }

    ProcEntry* back = front_;
    while (back->next != nullptr) {
      back = back->next;
    }

    back->next = new ProcEntry{
        .proc = proc,
        .next = nullptr,
    };
  }

  void DumpProcessStates() {
    ProcEntry* p = front_;
    dbgln("Process States:");
    while (p != nullptr) {
      dbgln("%u: %u", p->proc->id(), p->proc->GetState());
      p = p->next;
    }
  }

 private:
  struct ProcEntry {
    Process* proc;
    ProcEntry* next;
  };

  ProcEntry* front_ = nullptr;
};

class Scheduler {
 public:
  Scheduler() {
    Process* root = Process::RootProcess();
    runnable_threads_.PushBack(root->GetThread(0));
    proc_list_.InsertProcess(Process::RootProcess());
  }
  void Enable() { enabled_ = true; }

  Process& CurrentProcess() { return CurrentThread().process(); }
  Thread& CurrentThread() { return *runnable_threads_.PeekFront(); }

  void InsertProcess(Process* process) { proc_list_.InsertProcess(process); }
  void Enqueue(Thread* thread) { runnable_threads_.PushBack(thread); }

  void Yield() {
    if (!enabled_) {
      return;
    }
    asm volatile("cli");

    Thread* prev = nullptr;
    if (CurrentThread().GetState() == Thread::RUNNING) {
      prev = runnable_threads_.CycleFront();
      prev->SetState(Thread::RUNNABLE);
    } else {
      // This technically is a memory operation but should only occur when a
      // thread is blocking so may be ok?
      prev = runnable_threads_.PopFront();
    }

    if (runnable_threads_.size() == 0) {
      proc_list_.DumpProcessStates();
      panic("FIXME: Implement Sleep");
    }

    Thread* next = runnable_threads_.PeekFront();
    if (next->GetState() != Thread::RUNNABLE) {
      panic("Non-runnable thread in the queue");
    }
    // Needs to be before the next == prev check
    // otherwise the active thread will be RUNNABLE instead of RUNNING.
    next->SetState(Thread::RUNNING);

    if (next == prev) {
      dbgln("No next thread, continue");
      return;
    }

    context_switch(prev->Rsp0Ptr(), next->Rsp0Ptr());

    asm volatile("sti");
  }

 private:
  bool enabled_ = false;
  ProcList proc_list_;

  LinkedList<Thread*> runnable_threads_;
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
