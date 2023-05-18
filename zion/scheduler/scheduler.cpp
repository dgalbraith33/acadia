#include "scheduler/scheduler.h"

#include "debug/debug.h"

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
    current_thread_ = root->GetThread(0);
    proc_list_.InsertProcess(Process::RootProcess());
  }
  void Enable() { enabled_ = true; }

  Process& CurrentProcess() { return current_thread_->process(); }
  Thread& CurrentThread() { return *current_thread_; }

  void Enqueue(Thread* thread) {
    Thread* back = current_thread_;
    while (back->next_thread_ != nullptr) {
      back = back->next_thread_;
    }
    back->next_thread_ = thread;
  }

  void Yield() {
    if (!enabled_) {
      return;
    }
    asm volatile("cli");

    if (current_thread_->next_thread_ == nullptr) {
      dbgln("No next thread, continue");
      return;
    }

    Thread* prev = current_thread_;
    current_thread_ = current_thread_->next_thread_;
    prev->next_thread_ = nullptr;
    if (prev->pid() != 0) {
      Enqueue(prev);
    }
    context_switch(prev->Rsp0Ptr(), current_thread_->Rsp0Ptr());

    asm volatile("sti");
  }

 private:
  bool enabled_ = false;
  ProcList proc_list_;

  Thread* current_thread_;
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

void EnqueueThread(Thread* thread) { GetScheduler().Enqueue(thread); }

Process& CurrentProcess() { return GetScheduler().CurrentProcess(); }
Thread& CurrentThread() { return GetScheduler().CurrentThread(); }

}  // namespace sched
