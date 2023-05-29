#pragma once

#include <stdint.h>

#include "lib/linked_list.h"

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
  // Caller takes ownership of returned process.
  static Process* RootProcess();
  Process(uint64_t elf_ptr);

  uint64_t id() { return id_; }
  uint64_t cr3() { return cr3_; }

  void CreateThread(uint64_t elf_ptr);
  Thread* GetThread(uint64_t tid);

  // Checks the state of all child threads and transitions to
  // finished if all have finished.
  void CheckState();

  State GetState() { return state_; }

 private:
  Process(uint64_t id, uint64_t cr3) : id_(id), cr3_(cr3) {}
  uint64_t id_;
  uint64_t cr3_;
  State state_;

  uint64_t next_thread_id_ = 0;

  LinkedList<Thread*> threads_;
};
