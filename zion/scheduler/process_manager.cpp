#include "scheduler/process_manager.h"

#include "debug/debug.h"

ProcessManager* gProcMan = nullptr;

void ProcessManager::Init() {
  gProcMan = new ProcessManager();
  gProcMan->InsertProcess(Process::RootProcess());
}

void ProcessManager::InsertProcess(const glcr::RefPtr<Process>& proc) {
  proc_list_.PushBack(proc);
}

Process& ProcessManager::FromId(uint64_t pid) {
  if (pid >= proc_list_.size()) {
    panic("Bad proc access %u, have %u processes", pid, proc_list_.size());
  }
  return *proc_list_[pid];
}

void ProcessManager::DumpProcessStates() {
  dbgln("Process States: %u", proc_list_.size());
  for (uint64_t i = 0; i < proc_list_.size(); i++) {
    dbgln("%u: %u", proc_list_[i]->id(), proc_list_[i]->GetState());
  }
}
