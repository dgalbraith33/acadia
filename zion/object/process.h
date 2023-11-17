#pragma once

#include <glacier/container/vector.h>
#include <glacier/memory/ref_ptr.h>
#include <stdint.h>

#include "capability/capability.h"
#include "capability/capability_table.h"
#include "object/address_space.h"
#include "object/channel.h"
#include "object/mutex.h"
#include "object/port.h"

// Forward decl due to cyclic dependency.
class Thread;

template <>
struct KernelObjectTag<Process> {
  static const uint64_t type = KernelObject::PROCESS;
};

class Process : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::PROCESS; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Write | kZionPerm_Read | kZionPerm_SpawnThread |
           kZionPerm_SpawnProcess | kZionPerm_Duplicate | kZionPerm_Transmit;
  }

  enum State {
    UNSPECIFIED,
    SETUP,
    RUNNING,
    FINISHED,
  };
  static glcr::RefPtr<Process> RootProcess();
  static glcr::RefPtr<Process> Create();

  uint64_t id() const { return id_; }
  glcr::RefPtr<AddressSpace> vmas() { return vmas_; }

  glcr::RefPtr<Thread> CreateThread();
  glcr::RefPtr<Thread> GetThread(uint64_t tid);

  glcr::RefPtr<Capability> ReleaseCapability(uint64_t cid);
  glcr::RefPtr<Capability> GetCapability(uint64_t cid);

  template <typename T>
  uint64_t AddNewCapability(const glcr::RefPtr<T>& obj, uint64_t permissions) {
    return caps_.AddNewCapability(obj, permissions);
  }
  template <typename T>
  uint64_t AddNewCapability(const glcr::RefPtr<T>& obj) {
    return caps_.AddNewCapability(obj);
  }
  uint64_t AddExistingCapability(const glcr::RefPtr<Capability>& cap);

  // Checks the state of all child threads and transitions to
  // finished if all have finished.
  void CheckState();

  State GetState() { return state_; }

  void Exit();

 private:
  friend class glcr::MakeRefCountedFriend<Process>;
  Process();
  Process(uint64_t id) : id_(id), vmas_(AddressSpace::ForRoot()) {}

  glcr::RefPtr<Mutex> mutex_ = Mutex::Create();

  uint64_t id_;
  glcr::RefPtr<AddressSpace> vmas_;
  State state_;

  uint64_t next_thread_id_ = 0;

  glcr::Vector<glcr::RefPtr<Thread>> threads_;
  CapabilityTable caps_;
};
