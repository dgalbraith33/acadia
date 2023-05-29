#include "scheduler/scheduler.h"

#include "debug/debug.h"
#include "lib/linked_list.h"

namespace sched {
namespace {

extern "C" void context_switch(uint64_t* current_esp, uint64_t* next_esp);

void DumpProcessStates(LinkedList<Process*>& proc_list) {
  dbgln("Process States: %u", proc_list.size());
  auto iter = proc_list.begin();
  while (iter != proc_list.end()) {
    dbgln("%u: %u", iter->id(), iter->GetState());
    iter = iter.next();
  }
}

class Scheduler {
 public:
  Scheduler() {
    Process* root = Process::RootProcess();
    runnable_threads_.PushBack(root->GetThread(0));
    proc_list_.PushBack(Process::RootProcess());
  }
  void Enable() { enabled_ = true; }

  Process& CurrentProcess() { return CurrentThread().process(); }
  Thread& CurrentThread() { return *runnable_threads_.PeekFront(); }

  void InsertProcess(Process* process) { proc_list_.PushBack(process); }
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
      DumpProcessStates(proc_list_);
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
  // TODO: move this to a separate process manager class.
  LinkedList<Process*> proc_list_;
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
