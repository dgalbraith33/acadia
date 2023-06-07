#pragma once

#include <stdint.h>

#include "capability/capability.h"
#include "lib/linked_list.h"
#include "lib/ref_ptr.h"
#include "lib/shared_ptr.h"
#include "object/address_space.h"

// Forward decl due to cyclic dependency.
class Thread;

class Process : public KernelObject {
 public:
  enum State {
    UNSPECIFIED,
    SETUP,
    RUNNING,
    FINISHED,
  };
  static RefPtr<Process> RootProcess();
  static RefPtr<Process> Create();

  uint64_t id() const { return id_; }
  AddressSpace& vmm() { return vmm_; }

  RefPtr<Thread> CreateThread();
  RefPtr<Thread> GetThread(uint64_t tid);

  SharedPtr<Capability> GetCapability(uint64_t cid);
  uint64_t AddCapability(const RefPtr<Thread>& t);
  // Checks the state of all child threads and transitions to
  // finished if all have finished.
  void CheckState();

  State GetState() { return state_; }

 private:
  friend class MakeRefCountedFriend<Process>;
  Process();
  Process(uint64_t id) : id_(id), vmm_(AddressSpace::ForRoot()) {}
  uint64_t id_;
  AddressSpace vmm_;
  State state_;

  uint64_t next_thread_id_ = 0;
  uint64_t next_cap_id_ = 0x100;

  LinkedList<RefPtr<Thread>> threads_;
  LinkedList<SharedPtr<Capability>> caps_;
};
