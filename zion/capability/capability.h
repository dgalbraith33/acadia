#pragma once

#include <stdint.h>

#include "lib/ref_ptr.h"
#include "object/kernel_object.h"

class Process;
class Thread;

class Capability : public RefCounted<Capability> {
 public:
  Capability(const RefPtr<KernelObject>& obj, uint64_t permissions)
      : obj_(obj), permissions_(permissions) {}

  template <typename T>
  Capability(const RefPtr<T>& obj, uint64_t permissions)
      : Capability(StaticCastRefPtr<KernelObject>(obj), permissions) {}

  template <typename T>
  RefPtr<T> obj();

  RefPtr<KernelObject> raw_obj() { return obj_; }

  uint64_t permissions() { return permissions_; }
  bool HasPermissions(uint64_t requested) {
    return (permissions_ & requested) == requested;
  }

 private:
  RefPtr<KernelObject> obj_;
  uint64_t permissions_;
};

template <typename T>
RefPtr<T> Capability::obj() {
  if (obj_->TypeTag() != KernelObjectTag<T>::type) {
    return nullptr;
  }
  return StaticCastRefPtr<T>(obj_);
}

template <typename T>
z_err_t ValidateCapability(const RefPtr<Capability>& cap,
                           uint64_t permissions) {
  if (!cap) {
    return Z_ERR_CAP_NOT_FOUND;
  }

  if (cap->raw_obj()->TypeTag() != KernelObjectTag<T>::type) {
    return Z_ERR_CAP_TYPE;
  }

  if (!cap->HasPermissions(permissions)) {
    return Z_ERR_CAP_DENIED;
  }

  return Z_OK;
}

#define RET_IF_NULL(expr)    \
  {                          \
    if (!expr) {             \
      return Z_ERR_CAP_TYPE; \
    }                        \
  }
