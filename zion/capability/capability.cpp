#include "capability/capability.h"

#include "object/process.h"
#include "object/thread.h"

template <>
RefPtr<Process> Capability::obj<Process>() {
  if (type_ != PROCESS) {
    panic("Accessing %u cap as object.", type_);
  }
  return StaticCastRefPtr<Process>(obj_);
}

template <>
RefPtr<Thread> Capability::obj<Thread>() {
  if (type_ != THREAD) {
    panic("Accessing %u cap as object.", type_);
  }
  return StaticCastRefPtr<Thread>(obj_);
}

template <>
RefPtr<AddressSpace> Capability::obj<AddressSpace>() {
  if (type_ != ADDRESS_SPACE) {
    panic("Accessing %u cap as object.", type_);
  }
  return StaticCastRefPtr<AddressSpace>(obj_);
}

template <>
RefPtr<MemoryObject> Capability::obj<MemoryObject>() {
  if (type_ != MEMORY_OBJECT) {
    panic("Accessing %u cap as object.", type_);
  }
  return StaticCastRefPtr<MemoryObject>(obj_);
}

template <>
RefPtr<Channel> Capability::obj<Channel>() {
  if (type_ != CHANNEL) {
    panic("Accessing %u cap as object.", type_);
  }
  return StaticCastRefPtr<Channel>(obj_);
}
