#include "scheduler/cleanup.h"

void ProcessCleanup::CleanupLoop() {
  while (true) {
    while (process_list_.empty() && thread_list_.empty()) {
      semaphore_.Wait();
    }
    // TODO: I think we need to protect these lists with a mutex as well.
    while (!process_list_.empty()) {
      auto proc = process_list_.PopFront();
      dbgln("CLEANUP Proc {}", proc->id());
    }
    while (!thread_list_.empty()) {
      auto thread = thread_list_.PopFront();
      dbgln("CLEANUP Thread {}.{}", thread->pid(), thread->tid());
    }
  }
}

void ProcessCleanup::CleanupProcess(glcr::RefPtr<Process> process) {
  process_list_.PushBack(process);
  semaphore_.Signal();
}
void ProcessCleanup::CleanupThread(glcr::RefPtr<Thread> thread) {
  thread_list_.PushBack(thread);
  semaphore_.Signal();
}

void CleanupThreadEntry(ProcessCleanup* cleanup) {
  cleanup->CleanupLoop();
  UNREACHABLE;
}
