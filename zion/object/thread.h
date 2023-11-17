#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>
#include <stdint.h>

#include "include/ztypes.h"
#include "object/kernel_object.h"

// Forward decl due to cyclic dependency.
class Process;
class Thread;

template <>
struct KernelObjectTag<Thread> {
  static const uint64_t type = KernelObject::THREAD;
};

class Thread : public KernelObject, public glcr::IntrusiveListNode<Thread> {
 public:
  uint64_t TypeTag() override { return KernelObject::THREAD; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Read | kZionPerm_Write;
  }

  enum State {
    UNSPECIFIED,
    CREATED,
    RUNNING,
    RUNNABLE,
    BLOCKED,
    CLEANUP,
    FINISHED,
  };
  static glcr::RefPtr<Thread> RootThread(Process& root_proc);
  static glcr::RefPtr<Thread> Create(Process& proc, uint64_t tid);

  uint64_t tid() const { return id_; };
  uint64_t pid() const;

  Process& process() { return process_; }

  uint64_t* Rsp0Ptr() { return &rsp0_; }
  uint64_t Rsp0Start() { return rsp0_start_; }

  // Switches the thread's state to runnable and enqueues it.
  void Start(uint64_t entry, uint64_t arg1, uint64_t arg2);

  // Called the first time the thread starts up.
  void Init();

  // State Management.
  State GetState() { return state_; };
  void SetState(State state) { state_ = state; }
  bool IsDying() { return state_ == CLEANUP || state_ == FINISHED; }

  // Exits this thread.
  // Allows all blocked threads to run and releases the kernel stack.
  // This function should only be called by the running thread on itself
  // as it will yield.
  void Exit();

  // Like Exit except it does not yield.
  void Cleanup();

  void Wait();

 private:
  friend class glcr::MakeRefCountedFriend<Thread>;
  Thread(Process& proc, uint64_t tid);
  // Special constructor for the root thread only.
  Thread(Process& proc) : process_(proc), id_(0) {}
  Process& process_;
  uint64_t id_;
  State state_ = CREATED;

  // Startup Context for the thread.
  uint64_t rip_;
  uint64_t arg1_;
  uint64_t arg2_;

  // Stack pointer to take on resume.
  // Stack will contain the full thread context.
  uint64_t rsp0_;
  // Stack pointer to take when returning from userspace.
  // I don't think me mind clobbering the stack here.
  uint64_t rsp0_start_;

  glcr::IntrusiveList<Thread> blocked_threads_;
};
