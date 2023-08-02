#pragma once

#include <glacier/memory/ref_counted.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/status/error.h>
#include <stdint.h>

#include "include/ztypes.h"
#include "object/kernel_object.h"

class Process;
class Thread;

class Capability : public glcr::RefCounted<Capability> {
 public:
  Capability(const glcr::RefPtr<KernelObject>& obj, uint64_t permissions)
      : obj_(obj), permissions_(permissions) {}

  template <typename T>
  Capability(const glcr::RefPtr<T>& obj, uint64_t permissions)
      : Capability(StaticCastRefPtr<KernelObject>(obj), permissions) {}

  template <typename T>
  Capability(const glcr::RefPtr<T>& obj)
      : Capability(obj, T::DefaultPermissions()) {}

  template <typename T>
  glcr::RefPtr<T> obj();

  glcr::RefPtr<KernelObject> raw_obj() { return obj_; }

  uint64_t permissions() { return permissions_; }
  bool HasPermissions(uint64_t requested) {
    return (permissions_ & requested) == requested;
  }

 private:
  glcr::RefPtr<KernelObject> obj_;
  uint64_t permissions_;
};

template <typename T>
glcr::RefPtr<T> Capability::obj() {
  if (obj_->TypeTag() != KernelObjectTag<T>::type) {
    return nullptr;
  }
  return StaticCastRefPtr<T>(obj_);
}

template <typename T>
z_err_t ValidateCapability(const glcr::RefPtr<Capability>& cap,
                           uint64_t permissions) {
  if (!cap) {
    return glcr::CAP_NOT_FOUND;
  }

  if (cap->raw_obj()->TypeTag() != KernelObjectTag<T>::type) {
    return glcr::CAP_WRONG_TYPE;
  }

  if (!cap->HasPermissions(permissions)) {
    return glcr::CAP_PERMISSION_DENIED;
  }

  return glcr::OK;
}

#define RET_IF_NULL(expr)    \
  {                          \
    if (!expr) {             \
      return Z_ERR_CAP_TYPE; \
    }                        \
  }
