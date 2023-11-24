#pragma once

#include <glacier/container/linked_list.h>
#include <glacier/memory/ref_ptr.h>

#include "object/process.h"
#include "object/semaphore.h"
#include "object/thread.h"

class ProcessCleanup {
 public:
  ProcessCleanup() {}

  void CleanupLoop();

  void CleanupProcess(glcr::RefPtr<Process> process);
  void CleanupThread(glcr::RefPtr<Thread> thread);

 private:
  Semaphore semaphore_;
  glcr::LinkedList<glcr::RefPtr<Thread>> thread_list_;
  glcr::LinkedList<glcr::RefPtr<Process>> process_list_;
};

void CleanupThreadEntry(ProcessCleanup* cleanup);
