#include "scheduler/process.h"

#include "debug/debug.h"
#include "scheduler/thread.h"

namespace {

static uint64_t gNextId = 1;

}

Process* Process::RootProcess() {
  uint64_t pml4_addr = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(pml4_addr));
  Process* proc = new Process(0, pml4_addr);
  proc->thread_list_front_ = new ThreadEntry{
      .thread = Thread::RootThread(proc),
      .next = nullptr,
  };
  proc->next_thread_id_ = 1;

  return proc;
}

Thread* Process::CreateThread() {
  Thread* thread = new Thread(this, next_thread_id_++);

  ThreadEntry* entry = thread_list_front_;
  while (entry->next != nullptr) {
    entry = entry->next;
  }
  entry->next = new ThreadEntry{
      .thread = thread,
      .next = nullptr,
  };
  return thread;
}

Thread* Process::GetThread(uint64_t tid) {
  ThreadEntry* entry = thread_list_front_;
  while (entry != nullptr) {
    if (entry->thread->tid() == tid) {
      return entry->thread;
    }
  }
  panic("Bad thread access.");
  return nullptr;
}
