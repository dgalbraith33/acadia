#pragma once

#include "lib/linked_list.h"
#include "lib/shared_ptr.h"
#include "scheduler/process.h"

class ProcessManager {
 public:
  // Initializes the ProcessManager
  // and stores it in the global variable.
  static void Init();

  void InsertProcess(const SharedPtr<Process>& proc);
  Process& FromId(uint64_t id);

  void DumpProcessStates();

 private:
  // TODO: This should be a hashmap.
  LinkedList<SharedPtr<Process>> proc_list_;
};

extern ProcessManager* gProcMan;
