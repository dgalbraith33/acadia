#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>

#include "include/ztypes.h"
#include "object/kernel_object.h"
#include "object/thread.h"

class Mutex;

template <>
struct KernelObjectTag<Mutex> {
  static const uint64_t type = KernelObject::MUTEX;
};

class Mutex : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::MUTEX; }
  static uint64_t DefaultPermissions() {
    return kZionPerm_Lock | kZionPerm_Release;
  }

  static glcr::RefPtr<Mutex> Create();

  // Attempts to lock the mutex, suspends the current thread
  // until the lock is acquired.
  void Lock();

  // Releases the mutex and activates the next thread in the
  // blocked queue if it exists.
  void Release();

 private:
  uint8_t lock_ = 0;

  glcr::IntrusiveList<Thread> blocked_threads_;

  Mutex() {}
};

class MutexHolder {
 public:
  MutexHolder(const glcr::RefPtr<Mutex>& mutex) : mutex_(mutex) {
    mutex_->Lock();
  }

  ~MutexHolder() { mutex_->Release(); }

  MutexHolder(MutexHolder&) = delete;
  MutexHolder(MutexHolder&&) = delete;

 private:
  glcr::RefPtr<Mutex> mutex_;
};
