#pragma once

#include <glacier/container/linked_list.h>
#include <glacier/memory/ref_ptr.h>

#include "capability/capability.h"
#include "lib/mutex.h"

class CapabilityTable {
 public:
  CapabilityTable();

  CapabilityTable(CapabilityTable&) = delete;
  CapabilityTable& operator=(CapabilityTable&) = delete;

  template <typename T>
  z_cap_t AddNewCapability(const glcr::RefPtr<T>& object, uint64_t permissions);
  template <typename T>
  z_cap_t AddNewCapability(const glcr::RefPtr<T>& object) {
    return AddNewCapability<T>(object, T::DefaultPermissions());
  }
  z_cap_t AddExistingCapability(const glcr::RefPtr<Capability>& cap);

  glcr::RefPtr<Capability> GetCapability(uint64_t id);
  glcr::RefPtr<Capability> ReleaseCapability(uint64_t id);

 private:
  Mutex lock_{"cap table"};
  // TODO: Do some randomization.
  uint64_t next_cap_id_ = 0x100;
  // FIXME: use a map data structure.
  struct CapEntry {
    uint64_t id;
    glcr::RefPtr<Capability> cap;
  };
  glcr::LinkedList<CapEntry> capabilities_;
};

template <typename T>
uint64_t CapabilityTable::AddNewCapability(const glcr::RefPtr<T>& object,
                                           uint64_t permissions) {
  MutexHolder h(lock_);
  uint64_t id = next_cap_id_++;
  capabilities_.PushBack(
      {.id = id, .cap = MakeRefCounted<Capability>(object, permissions)});
  return id;
}
