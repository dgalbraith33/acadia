#pragma once

#include <stdint.h>

#include "debug/debug.h"

class Process;

class Capability {
 public:
  enum Type {
    UNDEFINED,
    PROCESS,
  };
  Capability(void* obj, Type type, uint64_t id, uint64_t permissions)
      : obj_(obj), type_(type), id_(id), permissions_(permissions) {}

  template <typename T>
  T& obj();

  uint64_t id() { return id_; }

  bool CheckType(Type type) { return type_ == type; }

  uint64_t permissions() { return permissions_; }
  bool HasPermissions(uint64_t requested) {
    return (permissions_ & requested) == requested;
  }

 private:
  // FIXME: This should somehow be a shared ptr to keep the object alive.
  void* obj_;
  Type type_;
  uint64_t id_;
  uint64_t permissions_;
};

template <class Process>
Process& Capability::obj() {
  if (type_ != PROCESS) {
    panic("Accessing %u cap as object.", type_);
  }
  return *static_cast<Process*>(obj_);
}