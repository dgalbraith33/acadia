#pragma once

#include <stdint.h>

namespace glcr {

template <typename T>
class ArrayIterator {
 public:
  ArrayIterator(T* item, uint64_t size) : item_(item), size_(size) {}

  ArrayIterator next() {
    if (size_ <= 1) {
      return {nullptr, 0};
    }
    return {item_ + 1, size_ - 1};
  }

  ArrayIterator& operator++() {
    if (size_ <= 1) {
      item_ = nullptr;
      size_ = 0;
    } else {
      item_++;
      size_--;
    }
    return *this;
  }

  T& operator*() { return *item_; }
  T* operator->() { return item_; }
  bool operator==(const ArrayIterator& other) { return item_ == other.item_; }
  bool operator!=(const ArrayIterator& other) { return item_ != other.item_; }

 private:
  T* item_;
  uint64_t size_;
};

}  // namespace glcr
