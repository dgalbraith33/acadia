#pragma once

#include <ztypes.h>

class Semaphore {
 public:
  Semaphore();
  ~Semaphore();

  void Wait();
  void Signal();

 private:
  z_cap_t semaphore_cap_;
};
