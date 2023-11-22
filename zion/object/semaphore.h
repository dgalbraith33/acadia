#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>

#include "include/ztypes.h"
#include "object/kernel_object.h"
#include "object/thread.h"

class Semaphore;

template <>
struct KernelObjectTag<Semaphore> {
  static const uint64_t type = KernelObject::SEMAPHORE;
};

class Semaphore : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::SEMAPHORE; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Wait | kZionPerm_Signal;
  }

  static glcr::RefPtr<Semaphore> Create();

  void Wait();
  void Signal();

 private:
  uint8_t lock_ = 0;

  glcr::IntrusiveList<Thread> blocked_threads_;
};
