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

  uint64_t At(uint64_t offset) const { return buffer_[offset]; }

  void WriteAt(uint64_t offset, uint64_t cap) { buffer_[offset] = cap; }

 private:
  uint64_t* buffer_;
};

}  // namespace glcr
