#pragma once

#include <stdint.h>

namespace glcr {

template <typename T>
class ArrayView {
 public:
  ArrayView() : data_(nullptr), size_(0) {}
  ArrayView(const ArrayView&) = default;
  ArrayView(T* data, uint64_t size) : data_(data), size_(size) {}

  T* RawPtr() { return data_; }
  const T* RawPtr() const { return data_; }

  uint64_t size() const { return size_; }

  T& operator[](uint64_t index) { return data_[index]; }
  const T& operator[](uint64_t index) const { return data_[index]; }

 private:
  T* data_;
  uint64_t size_;
};

}  // namespace glcr
