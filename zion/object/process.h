#pragma once

#include <stdint.h>

#include "capability/capability.h"
#include "capability/capability_table.h"
#include "lib/linked_list.h"
#include "lib/mutex.h"
#include "lib/ref_ptr.h"
#include "object/address_space.h"
#include "object/channel.h"
#include "object/port.h"

// Forward decl due to cyclic dependency.
class Thread;

template <>
struct KernelObjectTag<Process> {
  static const uint64_t type = KernelObject::PROCESS;
};

class Process : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::PROCESS; }
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
  uint64_t AddCapability(const RefPtr<Port>& chan);

  void AddCapability(uint64_t cap_id, const RefPtr<MemoryObject>& vmmo);
  // Checks the state of all child threads and transitions to
  // finished if all have finished.
  void CheckState();

  State GetState() { return state_; }

 private:
  friend class MakeRefCountedFriend<Process>;
  Process();
  Process(uint64_t id) : id_(id), vmas_(AddressSpace::ForRoot()) {}

  Mutex mutex_{"Process"};

  uint64_t id_;
  RefPtr<AddressSpace> vmas_;
  State state_;

  uint64_t next_thread_id_ = 0;
  uint64_t next_cap_id_ = 0x100;

  LinkedList<RefPtr<Thread>> threads_;
  CapabilityTable caps_;
};
