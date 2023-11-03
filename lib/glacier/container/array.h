#pragma once

#include <stdint.h>

#include "glacier/container/array_view.h"

namespace glcr {

template <typename T>
class Array {
 public:
  Array() : data_(nullptr), size_(0) {}

  explicit Array(uint64_t size) : data_(new T[size]), size_(size) {}

  Array(const ArrayView<T> view) : Array(view.size()) {
    for (uint64_t i = 0; i < size_; i++) {
      data_[i] = view[i];
    }
  }

  Array(const Array&) = delete;

  Array(Array&& other) : data_(other.data), size_(other.size_) {
    other.data_ = nullptr;
    other.size_ = 0;
  }

  Array& operator=(Array&& other) {
    if (data_) {
      delete[] data_;
    }
    data_ = other.data_;
    size_ = other.size_;
    other.data_ = nullptr;
    other.size_ = 0;
    return *this;
  }

  ~Array() {
    if (data_) {
      delete[] data_;
    }
  }

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
