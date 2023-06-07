#pragma once

#include "lib/linked_list.h"
#include "lib/ref_ptr.h"
#include "object/process.h"

class ProcessManager {
 public:
  // Initializes the ProcessManager
  // and stores it in the global variable.
  static void Init();

  void InsertProcess(const RefPtr<Process>& proc);
  Process& FromId(uint64_t id);

  void DumpProcessStates();

 private:
  // TODO: This should be a hashmap.
  LinkedList<RefPtr<Process>> proc_list_;
};

extern ProcessManager* gProcMan;
