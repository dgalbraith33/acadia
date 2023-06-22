#pragma once

#include <glacier/status/error.h>
#include <stdint.h>

class Thread {
 public:
  typedef void (*Entry)(void*);

  Thread() : thread_cap_(0) {}
  Thread(Entry e, const void* arg1);

  [[nodiscard]] glcr::ErrorCode Join();

 private:
  uint64_t thread_cap_;
};
