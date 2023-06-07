#pragma once

#include <stdint.h>

#include "lib/ref_ptr.h"
#include "object/kernel_object.h"

class Process;
class Thread;

class Capability : public RefCounted<Capability> {
 public:
  enum Type {
    UNDEFINED,
    PROCESS,
    THREAD,
    ADDRESS_SPACE,
    MEMORY_OBJECT,
  };
  Capability(const RefPtr<KernelObject>& obj, Type type, uint64_t id,
             uint64_t permissions)
      : obj_(obj), type_(type), id_(id), permissions_(permissions) {}

  template <typename T>
  Capability(const RefPtr<T>& obj, Type type, uint64_t id, uint64_t permissions)
      : Capability(StaticCastRefPtr<KernelObject>(obj), type, id, permissions) {
  }

  template <typename T>
  RefPtr<T> obj();

  uint64_t id() { return id_; }

  bool CheckType(Type type) { return type_ == type; }

  uint64_t permissions() { return permissions_; }
  bool HasPermissions(uint64_t requested) {
    return (permissions_ & requested) == requested;
  }

 private:
  RefPtr<KernelObject> obj_;
  Type type_;
  uint64_t id_;
  uint64_t permissions_;
};
