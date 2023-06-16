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

  template <typename T>
  uint64_t AddNewCapability(const RefPtr<T>& obj, uint64_t permissions) {
    return caps_.AddNewCapability(obj, permissions);
  }
  uint64_t AddExistingCapability(const RefPtr<Capability>& cap);

  // FIXME: Eliminate reliance on this.
  template <typename T>
  void AddNewCapabilityWithId(uint64_t id, const RefPtr<T>& obj,
                              uint64_t permissions) {
    return caps_.AddNewCapabilityWithId(id, obj, permissions);
  }

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
