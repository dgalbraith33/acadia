#pragma once

#include <stdint.h>

#include "debug/debug.h"

template <typename T>
class RefCounted {
 public:
  RefCounted() {}
  ~RefCounted() { dbgln("RefCounted object destroyed"); }
  void Adopt() {
    if (ref_count_ != -1) {
      panic("Adopting owned ptr");
    } else {
      ref_count_ = 1;
    }
  }

  void Acquire() {
    if (ref_count_ == -1) {
      panic("Acquiring unowned ptr");
    }
    ref_count_++;
  }
  bool Release() {
    if (ref_count_ == -1 || ref_count_ == 0) {
      panic("Releasing unowned ptr");
    }
    return (--ref_count_) == 0;
  }

 private:
  // FIXME: This should be an atomic type.
  uint64_t ref_count_ = -1;
  // Disallow copy and move.
  RefCounted(RefCounted&) = delete;
  RefCounted(RefCounted&&) = delete;
  RefCounted& operator=(RefCounted&) = delete;
  RefCounted& operator=(RefCounted&&) = delete;
};
