#include "scheduler/scheduler.h"

#include "debug/debug.h"

namespace sched {
namespace {

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

  void Yield() {}

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

Process& CurrentProcess() { return GetScheduler().CurrentProcess(); }
Thread& CurrentThread() { return GetScheduler().CurrentThread(); }

}  // namespace sched
