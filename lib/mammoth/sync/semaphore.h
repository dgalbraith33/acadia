#pragma once

#include <ztypes.h>

namespace mmth {

class Semaphore {
 public:
  Semaphore();
  ~Semaphore();

  void Wait();
  void Signal();

 private:
  z_cap_t semaphore_cap_;
};

}  // namespace mmth
