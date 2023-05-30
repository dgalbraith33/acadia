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
  uint64_t pml4_addr = 0;
  asm volatile("mov %%cr3, %0;" : "=r"(pml4_addr));
  SharedPtr<Process> proc(new Process(0, pml4_addr));
  proc->threads_.PushBack(Thread::RootThread(proc.ptr()));
  proc->next_thread_id_ = 1;

  return proc;
}

Process::Process(uint64_t elf_ptr) : id_(gNextId++), state_(RUNNING) {
  cr3_ = phys_mem::AllocatePage();
  InitializePml4(cr3_);
  CreateThread(elf_ptr);
}

void Process::CreateThread(uint64_t elf_ptr) {
  Thread* thread = new Thread(this, next_thread_id_++, elf_ptr);
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
