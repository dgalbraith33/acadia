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
