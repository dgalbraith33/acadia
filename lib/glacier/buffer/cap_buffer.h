#pragma once

#include <stdint.h>

namespace glcr {

// TODO: Hold cap type instead of uint64_t
class CapBuffer {
 public:
  CapBuffer(uint64_t size) : buffer_(new uint64_t[size]) {}
  CapBuffer(const CapBuffer&) = delete;
  CapBuffer(CapBuffer&&) = delete;

  ~CapBuffer() { delete[] buffer_; }

  void Reset() {
    // FIXME: Zero out caps here?
    used_slots_ = 0;
  }

  uint64_t* RawPtr() { return buffer_; }

  uint64_t UsedSlots() { return used_slots_; }

  uint64_t At(uint64_t offset) const { return buffer_[offset]; }

  void WriteAt(uint64_t offset, uint64_t cap) {
    buffer_[offset] = cap;
    // This is fairly hacky considering we pass out the raw ptr.
    if (used_slots_ < (offset + 1)) {
      used_slots_ = offset + 1;
    }
  }

 private:
  uint64_t* buffer_;
  uint64_t used_slots_ = 0;
};

}  // namespace glcr
