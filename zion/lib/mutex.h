#pragma once

#include <stdint.h>

class Mutex {
 public:
  Mutex(const char* name) : name_(name) {}

  // FIXME: Block thread on lock rather than "preempting"
  void Lock();
  void Unlock() { lock_ = false; }

 private:
  const char* name_;

  uint8_t lock_ = 0;
};

class MutexHolder {
 public:
  MutexHolder(Mutex& mutex) : mutex_(mutex) { mutex_.Lock(); }

  ~MutexHolder() { mutex_.Unlock(); }

  MutexHolder(MutexHolder&) = delete;
  MutexHolder(MutexHolder&&) = delete;

 private:
  Mutex& mutex_;
};
