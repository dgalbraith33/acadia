#pragma once

#include <stdint.h>

// Forward decl due to cyclic dependency.
class Process;

class Thread {
 public:
  Thread(Process* proc, uint64_t thread_id) : process_(proc), id_(thread_id) {}

  uint64_t tid() { return id_; };
  uint64_t pid();

  Process& process() { return *process_; }

 private:
  Process* process_;
  uint64_t id_;

  // Next task in queue for quick scheduling.
  Thread* next_task_;
};
