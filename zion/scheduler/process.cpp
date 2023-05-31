#include "scheduler/process.h"

#include "debug/debug.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"
#include "scheduler/thread.h"

namespace {

static uint64_t gNextId = 1;

}

SharedPtr<Process> Process::RootProcess() {
  SharedPtr<Process> proc(new Process(0));
  proc->threads_.PushBack(Thread::RootThread(*proc));
  proc->next_thread_id_ = 1;

  return proc;
}

Process::Process() : id_(gNextId++), state_(RUNNING) {}

void Process::CreateThread(uint64_t entry) {
  Thread* thread = new Thread(*this, next_thread_id_++, entry);
  threads_.PushBack(thread);
  gScheduler->Enqueue(thread);
}

SharedPtr<Thread> Process::GetThread(uint64_t tid) {
  auto iter = threads_.begin();
  while (iter != threads_.end()) {
    if (iter->tid() == tid) {
      return *iter;
    }
    ++iter;
  }
  panic("Bad thread access.");
  return nullptr;
}

void Process::CheckState() {
  auto iter = threads_.begin();
  while (iter != threads_.end()) {
    if (iter->GetState() != Thread::FINISHED) {
      return;
    }
    ++iter;
  }
  state_ = FINISHED;
}
