#include "capability/capability_table.h"

CapabilityTable::CapabilityTable() {}

uint64_t CapabilityTable::AddExistingCapability(const RefPtr<Capability>& cap) {
  MutexHolder h(lock_);
  cap->set_id(next_cap_id_++);
  capabilities_.PushBack(cap);
  return cap->id();
}

RefPtr<Capability> CapabilityTable::GetCapability(uint64_t id) {
  MutexHolder h(lock_);
  auto iter = capabilities_.begin();
  while (iter != capabilities_.end()) {
    if (*iter && (*iter)->id() == id) {
      return *iter;
    }
    ++iter;
  }
  dbgln("Bad cap access %u", id);
  dbgln("Num caps: %u", capabilities_.size());
  return {};
}

RefPtr<Capability> CapabilityTable::ReleaseCapability(uint64_t id) {
  MutexHolder h(lock_);
  auto iter = capabilities_.begin();
  while (iter != capabilities_.end()) {
    if (*iter && (*iter)->id() == id) {
      // FIXME: Do an actual release here.
      auto cap = *iter;
      *iter = {nullptr};
      return cap;
    }
    ++iter;
  }
  dbgln("Bad cap release: %u", id);
  dbgln("Num caps: %u", capabilities_.size());
  return {};
}
