#pragma once

#include <stdint.h>

#include "capability/capability.h"
#include "lib/linked_list.h"
#include "lib/ref_ptr.h"
#include "object/address_space.h"
#include "object/channel.h"

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
  RefPtr<AddressSpace> vmas() { return vmas_; }

  RefPtr<Thread> CreateThread();
  RefPtr<Thread> GetThread(uint64_t tid);

  RefPtr<Capability> ReleaseCapability(uint64_t cid);
  RefPtr<Capability> GetCapability(uint64_t cid);
  // FIXME: We can't reset the cap id here.
  uint64_t AddCapability(const RefPtr<Capability>& cap);
  uint64_t AddCapability(const RefPtr<Thread>& t);
  uint64_t AddCapability(const RefPtr<Process>& p);
  uint64_t AddCapability(const RefPtr<AddressSpace>& vmas);
  uint64_t AddCapability(const RefPtr<MemoryObject>& vmmo);
  uint64_t AddCapability(const RefPtr<Channel>& chan);

  void AddCapability(uint64_t cap_id, const RefPtr<MemoryObject>& vmmo);
  // Checks the state of all child threads and transitions to
  // finished if all have finished.
  void CheckState();

  State GetState() { return state_; }

 private:
  friend class MakeRefCountedFriend<Process>;
  Process();
  Process(uint64_t id) : id_(id), vmas_(AddressSpace::ForRoot()) {}
  uint64_t id_;
  RefPtr<AddressSpace> vmas_;
  State state_;

  uint64_t next_thread_id_ = 0;
  uint64_t next_cap_id_ = 0x100;

  LinkedList<RefPtr<Thread>> threads_;
  LinkedList<RefPtr<Capability>> caps_;
};