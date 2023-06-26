#include "capability/capability_table.h"

#include "debug/debug.h"

CapabilityTable::CapabilityTable() {}

uint64_t CapabilityTable::AddExistingCapability(
    const glcr::RefPtr<Capability>& cap) {
  MutexHolder h(lock_);
  uint64_t id = next_cap_id_++;
  capabilities_.PushBack({.id = id, .cap = cap});
  return id;
}

glcr::RefPtr<Capability> CapabilityTable::GetCapability(uint64_t id) {
  MutexHolder h(lock_);
  auto iter = capabilities_.begin();
  while (iter != capabilities_.end()) {
    if (iter->cap && iter->id == id) {
      return iter->cap;
    }
    ++iter;
  }
  dbgln("Bad cap access %u", id);
  dbgln("Num caps: %u", capabilities_.size());
  return {};
}

glcr::RefPtr<Capability> CapabilityTable::ReleaseCapability(uint64_t id) {
  MutexHolder h(lock_);
  auto iter = capabilities_.begin();
  while (iter != capabilities_.end()) {
    if (iter->cap && iter->id == id) {
      // FIXME: Do an actual release here.
      auto cap = iter->cap;
      iter->cap = {nullptr};
      return cap;
    }
    ++iter;
  }
  dbgln("Bad cap release: %u", id);
  dbgln("Num caps: %u", capabilities_.size());
  return {};
}
