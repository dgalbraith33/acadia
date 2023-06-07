#include "capability/capability.h"

#include "object/process.h"
#include "object/thread.h"

template <>
Process& Capability::obj<Process>() {
  if (type_ != PROCESS) {
    panic("Accessing %u cap as object.", type_);
  }
  return *static_cast<Process*>(obj_.get());
}

template <>
Thread& Capability::obj<Thread>() {
  if (type_ != THREAD) {
    panic("Accessing %u cap as object.", type_);
  }
  return *static_cast<Thread*>(obj_.get());
}
