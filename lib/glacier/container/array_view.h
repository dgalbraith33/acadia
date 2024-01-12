#pragma once

#include <stdint.h>

#include "glacier/container/array_iter.h"

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

  typedef ArrayIterator<T> Iterator;

  Iterator begin() {
    if (size_ == 0) {
      return {nullptr, 0};
    }
    return {data_, size_};
  }
  const Iterator begin() const {
    if (size_ == 0) {
      return {nullptr, 0};
    }
    return {data_, size_};
  }
  Iterator end() { return {nullptr, 0}; }
  const Iterator end() const { return {nullptr, 0}; }

 private:
  T* data_;
  uint64_t size_;
};

}  // namespace glcr
