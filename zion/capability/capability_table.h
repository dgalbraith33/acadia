#pragma once

#include <glacier/container/hash_map.h>
#include <glacier/memory/ref_ptr.h>

#include "capability/capability.h"
#include "debug/debug.h"
#include "object/mutex.h"

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
  glcr::RefPtr<Mutex> lock_ = Mutex::Create();
  // TODO: Do some randomization.
  uint64_t next_cap_id_ = 0x100;
  // TODO: Consider not holding a uniqueptr here instead of a refptr?
  glcr::HashMap<uint64_t, glcr::RefPtr<Capability>> capabilities_;
};

template <typename T>
uint64_t CapabilityTable::AddNewCapability(const glcr::RefPtr<T>& object,
                                           uint64_t permissions) {
  MutexHolder h(lock_);
  uint64_t id = next_cap_id_++;
  if (capabilities_.Insert(
          id, MakeRefCounted<Capability>(object, permissions)) != glcr::OK) {
    panic("Reusing capability id {}", id);
  }
  return id;
}
