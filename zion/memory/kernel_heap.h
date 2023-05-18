#pragma once

#include <stdint.h>

class KernelHeap {
 public:
  KernelHeap(uint64_t lower_bound, uint64_t upper_bound);

  void* Allocate(uint64_t size);

 private:
  uint64_t next_addr_;
  uint64_t upper_bound_;
};
