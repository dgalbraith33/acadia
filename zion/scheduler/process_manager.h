#pragma once

#include <glacier/container/linked_list.h>
#include <glacier/memory/ref_ptr.h>

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
  glcr::LinkedList<glcr::RefPtr<Process>> proc_list_;
};

extern ProcessManager* gProcMan;
