#pragma once

#include <stdint.h>

#include "lib/linked_list.h"
#include "lib/shared_ptr.h"
#include "memory/virtual_memory.h"

// Forward decl due to cyclic dependency.
class Thread;

class Process {
 public:
  enum State {
    UNSPECIFIED,
    SETUP,
    RUNNING,
    FINISHED,
  };
  static SharedPtr<Process> RootProcess();
  Process();

  uint64_t id() const { return id_; }
  VirtualMemory& vmm() { return vmm_; }

  void CreateThread(uint64_t entry);
  SharedPtr<Thread> GetThread(uint64_t tid);

  // Checks the state of all child threads and transitions to
  // finished if all have finished.
  void CheckState();

  State GetState() { return state_; }

 private:
  Process(uint64_t id) : id_(id), vmm_(VirtualMemory::ForRoot()) {}
  uint64_t id_;
  VirtualMemory vmm_;
  State state_;

  uint64_t next_thread_id_ = 0;

  LinkedList<SharedPtr<Thread>> threads_;
};
