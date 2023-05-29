#pragma once

#include <stdint.h>

// Forward decl due to cyclic dependency.
class Process;

class Thread {
 public:
  static Thread* RootThread(Process* root_proc);

  explicit Thread(Process* proc, uint64_t tid, uint64_t elf_ptr);

  uint64_t tid() { return id_; };
  uint64_t pid();

  Process& process() { return *process_; }

  uint64_t* Rsp0Ptr() { return &rsp0_; }
  uint64_t Rsp0Start() { return rsp0_start_; }

  // Called the first time the thread starts up.
  void Init();

  // FIXME: Probably make this private.
  Thread* next_thread_;

 private:
  // Special constructor for the root thread only.
  Thread(Process* proc) : process_(proc), id_(0) {}
  Process* process_;
  uint64_t id_;

  uint64_t elf_ptr_;

  // Stack pointer to take on resume.
  // Stack will contain the full thread context.
  uint64_t rsp0_;
  // Stack pointer to take when returning from userspace.
  // I don't think me mind clobbering the stack here.
  uint64_t rsp0_start_;
};
