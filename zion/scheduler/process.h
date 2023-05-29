#pragma once

#include <stdint.h>

// Forward decl due to cyclic dependency.
class Thread;

class Process {
 public:
  // Caller takes ownership of returned process.
  static Process* RootProcess();
  Process(uint64_t elf_ptr);

  uint64_t id() { return id_; }
  uint64_t cr3() { return cr3_; }

  void CreateThread(uint64_t elf_ptr);
  Thread* GetThread(uint64_t tid);

 private:
  Process(uint64_t id, uint64_t cr3) : id_(id), cr3_(cr3) {}
  uint64_t id_;
  uint64_t cr3_;

  uint64_t next_thread_id_ = 0;

  // FIXME: Make a better data structure for this.
  struct ThreadEntry {
    Thread* thread;
    ThreadEntry* next;
  };
  ThreadEntry* thread_list_front_;
};
