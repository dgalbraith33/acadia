#include "capability/capability.h"

template <>
Process& Capability::obj<Process>() {
  if (type_ != PROCESS) {
    panic("Accessing %u cap as object.", type_);
  }
  return *static_cast<Process*>(obj_);
}

template <>
Thread& Capability::obj<Thread>() {
  if (type_ != THREAD) {
    panic("Accessing %u cap as object.", type_);
  }
  return *static_cast<Thread*>(obj_);
}