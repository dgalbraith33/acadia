#pragma once

#include "capability/capability.h"
#include "lib/linked_list.h"
#include "lib/mutex.h"
#include "lib/ref_ptr.h"

class CapabilityTable {
 public:
  CapabilityTable();

  CapabilityTable(CapabilityTable&) = delete;
  CapabilityTable& operator=(CapabilityTable&) = delete;

  template <typename T>
  uint64_t AddNewCapability(const RefPtr<T>& object, uint64_t permissions);
  uint64_t AddExistingCapability(const RefPtr<Capability>& cap);

  RefPtr<Capability> GetCapability(uint64_t id);
  RefPtr<Capability> ReleaseCapability(uint64_t id);

 private:
  Mutex lock_{"cap table"};
  // TODO: Do some randomization.
  uint64_t next_cap_id_ = 0x100;
  // FIXME: use a map data structure.
  struct CapEntry {
    uint64_t id;
    RefPtr<Capability> cap;
  };
  LinkedList<CapEntry> capabilities_;
};

template <typename T>
uint64_t CapabilityTable::AddNewCapability(const RefPtr<T>& object,
                                           uint64_t permissions) {
  MutexHolder h(lock_);
  uint64_t id = next_cap_id_++;
  capabilities_.PushBack(
      {.id = id, .cap = MakeRefCounted<Capability>(object, permissions)});
  return id;
}
