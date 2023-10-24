#pragma once

#include <stdint.h>

#include "glacier/string/string.h"

namespace glcr {

class ByteBuffer {
 public:
  ByteBuffer(uint64_t size) : buffer_(new uint8_t[size]) {}
  ByteBuffer(const ByteBuffer&) = delete;
  ByteBuffer(ByteBuffer&&) = delete;

  ~ByteBuffer() { delete[] buffer_; }

  template <typename T>
  void WriteAt(uint64_t offset, const T& object) {
    // FIXME: Add bounds check here.
    *reinterpret_cast<T*>(buffer_ + offset) = object;
  }

  void WriteStringAt(uint64_t offset, const String& string) {
    for (uint64_t i = 0; i < string.length(); i++) {
      buffer_[offset + i] = string[i];
    }
  }

  template <typename T>
  const T& At(uint64_t offset) const {
    return *reinterpret_cast<T*>(buffer_ + offset);
  }

  String StringAt(uint64_t offset, uint64_t length) const {
    return String(reinterpret_cast<char*>(buffer_ + offset), length);
  }

  // private:
  uint8_t* buffer_;
};

}  // namespace glcr
