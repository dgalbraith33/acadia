#pragma once

#include <stdint.h>

namespace glcr {

template <typename T>
class ArrayView {
 public:
  ArrayView() : data_(nullptr), size_(0) {}

  ArrayView(const ArrayView&) = default;
  ArrayView& operator=(const ArrayView&) = default;
  ArrayView(ArrayView&&) = default;
  ArrayView& operator=(ArrayView&&) = default;

  ArrayView(T* data, uint64_t size) : data_(data), size_(size) {}

  // Accessors.
  T& operator[](uint64_t index) { return data_[index]; }
  const T& operator[](uint64_t index) const { return data_[index]; }

  T* RawPtr() { return data_; }
  const T* RawPtr() const { return data_; }

  uint64_t size() const { return size_; }
  bool empty() const { return size_; }

 private:
  T* data_;
  uint64_t size_;
};

}  // namespace glcr
