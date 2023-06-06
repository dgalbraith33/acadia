#pragma once

#include <stdint.h>

class Thread {
 public:
  typedef void (*Entry)(void*);

  Thread(Entry e, const void* arg1);

 private:
  uint64_t thread_cap_;
};
