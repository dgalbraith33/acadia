#include "scheduler/process_manager.h"

#include "debug/debug.h"

ProcessManager* gProcMan = nullptr;

void ProcessManager::Init() {
  gProcMan = new ProcessManager();
  gProcMan->InsertProcess(Process::RootProcess());
}

void ProcessManager::InsertProcess(const glcr::RefPtr<Process>& proc) {
  PANIC_ON_ERR(proc_map_.Insert(proc->id(), proc), "Reinserting process");
}

Process& ProcessManager::FromId(uint64_t pid) {
  if (!proc_map_.Contains(pid)) {
    panic("Bad proc access {}, have {} processes", pid, proc_map_.size());
  }
  return *proc_map_.at(pid);
}

void ProcessManager::InitCleanup() {
  auto cleanup_thread = FromId(0).CreateThread();
  cleanup_thread->SetKernel();
  cleanup_thread->Start(reinterpret_cast<uint64_t>(CleanupThreadEntry),
                        reinterpret_cast<uint64_t>(&gProcMan->cleanup), 0);
}

void ProcessManager::CleanupProcess(uint64_t pid) {
  if (!proc_map_.Contains(pid)) {
    panic("Bad proc access {}, have {} processes", pid, proc_map_.size());
  }
  cleanup.CleanupProcess(proc_map_.at(pid));
}

void ProcessManager::CleanupThread(glcr::RefPtr<Thread> thread) {
  cleanup.CleanupThread(thread);
}
