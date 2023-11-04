#pragma once

#include "glacier/memory/move.h"

namespace glcr {

template <typename T>
class Optional {
 public:
  Optional() : empty_(nullptr), has_value_(false) {}
  Optional(const T& value) : value_(value), has_value_(true) {}
  Optional(T&& value) : value_(Move(value)), has_value_(true) {}
  Optional(const Optional&) = default;
  Optional(Optional&&) = default;
  ~Optional() {
    if (has_value_) {
      value_.~T();
    }
  }

  bool empty() const { return !has_value_; }
  explicit operator bool() { return has_value_; }

  const T& value() const { return value_; }
  T&& release_value() {
    has_value_ = false;
    return Move(value_);
  }

  T* operator->() { return &value_; }
  T& operator*() { return value_; }

 private:
  union {
    T value_;
    void* empty_;
  };
  bool has_value_;
};

}  // namespace glcr
