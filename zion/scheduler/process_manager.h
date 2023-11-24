#pragma once

#include <glacier/container/vector.h>
#include <glacier/memory/ref_ptr.h>

#include "object/process.h"
#include "scheduler/cleanup.h"

class ProcessManager {
 public:
  // Initializes the ProcessManager
  // and stores it in the global variable.
  static void Init();

  void InsertProcess(const glcr::RefPtr<Process>& proc);
  void RemoveProcess(uint64_t id);

  Process& FromId(uint64_t id);

  void InitCleanup();

  void CleanupProcess(uint64_t pid);
  void CleanupThread(glcr::RefPtr<Thread> thread);

 private:
  glcr::HashMap<uint64_t, glcr::RefPtr<Process>> proc_map_;
  ProcessCleanup cleanup;
};

extern ProcessManager* gProcMan;
