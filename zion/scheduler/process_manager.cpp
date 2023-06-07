#include "scheduler/process_manager.h"

ProcessManager* gProcMan = nullptr;

void ProcessManager::Init() {
  gProcMan = new ProcessManager();
  gProcMan->InsertProcess(Process::RootProcess());
}

void ProcessManager::InsertProcess(const RefPtr<Process>& proc) {
  proc_list_.PushBack(proc);
}

Process& ProcessManager::FromId(uint64_t pid) {
  auto iter = proc_list_.begin();
  while (iter != proc_list_.end()) {
    if (iter->id() == pid) {
      return **iter;
    }
    ++iter;
  }

  panic("Searching for invalid process id");
  return *((Process*)0);
}

void ProcessManager::DumpProcessStates() {
  dbgln("Process States: %u", proc_list_.size());
  auto iter = proc_list_.begin();
  while (iter != proc_list_.end()) {
    dbgln("%u: %u", iter->id(), iter->GetState());
    ++iter;
  }
}
