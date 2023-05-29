#include "scheduler/process.h"

#include "debug/debug.h"
#include "memory/paging_util.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"
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

Process::Process(uint64_t elf_ptr) : id_(gNextId++) {
  cr3_ = phys_mem::AllocatePage();
  InitializePml4(cr3_);
  CreateThread(elf_ptr);
}

void Process::CreateThread(uint64_t elf_ptr) {
  Thread* thread = new Thread(this, next_thread_id_++, elf_ptr);
  ThreadEntry* tentry = new ThreadEntry{
      .thread = thread,
      .next = nullptr,
  };

  if (thread_list_front_ == nullptr) {
    thread_list_front_ = tentry;
  } else {
    ThreadEntry* entry = thread_list_front_;
    while (entry->next != nullptr) {
      entry = entry->next;
    }
    entry->next = tentry;
  }
  sched::EnqueueThread(thread);
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
