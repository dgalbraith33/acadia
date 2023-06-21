#pragma once

#include <glacier/memory/ref_ptr.h>

#include "lib/linked_list.h"
#include "object/process.h"

class ProcessManager {
 public:
  // Initializes the ProcessManager
  // and stores it in the global variable.
  static void Init();

  void InsertProcess(const glcr::RefPtr<Process>& proc);
  Process& FromId(uint64_t id);

  void DumpProcessStates();

 private:
  // TODO: This should be a hashmap.
  LinkedList<glcr::RefPtr<Process>> proc_list_;
};

extern ProcessManager* gProcMan;
