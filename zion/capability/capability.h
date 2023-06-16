#pragma once

#include <stdint.h>

#include "lib/ref_ptr.h"
#include "object/kernel_object.h"

class Process;
class Thread;

class Capability : public RefCounted<Capability> {
 public:
  Capability(const RefPtr<KernelObject>& obj, uint64_t id, uint64_t permissions)
      : obj_(obj), id_(id), permissions_(permissions) {}

  template <typename T>
  Capability(const RefPtr<T>& obj, uint64_t id, uint64_t permissions)
      : Capability(StaticCastRefPtr<KernelObject>(obj), id, permissions) {}

  template <typename T>
  RefPtr<T> obj();

  uint64_t id() { return id_; }
  void set_id(uint64_t id) { id_ = id; }

  uint64_t permissions() { return permissions_; }
  bool HasPermissions(uint64_t requested) {
    return (permissions_ & requested) == requested;
  }

 private:
  RefPtr<KernelObject> obj_;
  uint64_t id_;
  uint64_t permissions_;
};

template <typename T>
RefPtr<T> Capability::obj() {
  if (obj_->TypeTag() != KernelObjectTag<T>::type) {
    return nullptr;
  }
  return StaticCastRefPtr<T>(obj_);
}
