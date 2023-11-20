#include "capability/capability_table.h"

#include "debug/debug.h"

CapabilityTable::CapabilityTable() {}

uint64_t CapabilityTable::AddExistingCapability(
    const glcr::RefPtr<Capability>& cap) {
  MutexHolder h(lock_);
  uint64_t id = next_cap_id_++;
  if (capabilities_.Insert(id, cap) != glcr::OK) {
    panic("Reusing capability id.");
  }
  return id;
}

glcr::RefPtr<Capability> CapabilityTable::GetCapability(uint64_t id) {
  MutexHolder h(lock_);
  if (!capabilities_.Contains(id)) {
    dbgln("Bad cap access {}", id);
    return {};
  }
  return capabilities_.at(id);
}

glcr::RefPtr<Capability> CapabilityTable::ReleaseCapability(uint64_t id) {
  MutexHolder h(lock_);
  if (!capabilities_.Contains(id)) {
    dbgln("Bad cap release {}", id);
    return {};
  }
  auto cap = capabilities_.at(id);
  (void)capabilities_.Delete(id);
  return cap;
}
