#pragma once

#include <stdint.h>

#include "glacier/container/array_view.h"

namespace glcr {

template <typename T>
class Array {
 public:
  // Constructors.
  Array() : data_(nullptr), size_(0) {}

  Array(const Array&) = delete;
  Array& operator=(const Array&) = delete;

  Array(Array&&);
  Array& operator=(Array&&);

  explicit Array(uint64_t size) : data_(new T[size]), size_(size) {}
  Array(const ArrayView<T>& view);

  ~Array() {
    if (data_) {
      delete[] data_;
    }
  }

  // Accessors.
  T& operator[](uint64_t index) { return data_[index]; }
  const T& operator[](uint64_t index) const { return data_[index]; }

  T* RawPtr() { return data_; }
  const T* RawPtr() const { return data_; }

  uint64_t size() const { return size_; }
  bool empty() const { return size_ == 0; }

 private:
  T* data_;
  uint64_t size_;
};

template <typename T>
Array<T>::Array(Array&& other) : data_(other.data_), size_(other.size_) {
  other.data_ = nullptr;
  other.size_ = 0;
}

template <typename T>
Array<T>& Array<T>::operator=(Array&& other) {
  if (data_) {
    delete[] data_;
  }
  data_ = other.data_;
  size_ = other.size_;
  other.data_ = nullptr;
  other.size_ = 0;
  return *this;
}

template <typename T>
Array<T>::Array(const ArrayView<T>& view) : Array(view.size()) {
  for (uint64_t i = 0; i < size_; i++) {
    data_[i] = view[i];
  }
}

}  // namespace glcr
