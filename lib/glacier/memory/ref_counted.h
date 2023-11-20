#pragma once

#include <stdint.h>

namespace glcr {

template <typename T>
class RefCounted {
 public:
  RefCounted() {}
  virtual ~RefCounted() {}
  // FIXME: Rethink error handling in these cases now that we can't panic the
  // kernel.
  void AdoptPtr() { ref_count_ = 1; }

  void AcquirePtr() { ref_count_++; }
  bool ReleasePtr() { return (--ref_count_) == 0; }

  uint64_t ref_count() { return ref_count_; }

 private:
  // FIXME: This should be an atomic type.
  uint64_t ref_count_ = -1;
  // Disallow copy and move.
  RefCounted(RefCounted&) = delete;
  RefCounted(RefCounted&&) = delete;
  RefCounted& operator=(RefCounted&) = delete;
  RefCounted& operator=(RefCounted&&) = delete;
};

}  // namespace glcr
