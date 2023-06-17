#pragma once

#include <stdint.h>

#include "debug/debug.h"

template <typename T>
class SharedPtr {
 public:
  SharedPtr() : init_(false), ptr_(0), ref_cnt_(0) {}
  // Takes ownership.
  SharedPtr(T* ptr) {
    ptr_ = ptr;
    ref_cnt_ = new uint64_t(1);
  }

  SharedPtr(const SharedPtr<T>& other)
      : init_(other.init_), ptr_(other.ptr_), ref_cnt_(other.ref_cnt_) {
    (*ref_cnt_)++;
  }

  SharedPtr& operator=(const SharedPtr<T>& other) {
    Cleanup();
    init_ = other.init_;
    ptr_ = other.ptr_;
    ref_cnt_ = other.ref_cnt_;
    (*ref_cnt_)++;
    return *this;
  }

  ~SharedPtr() { Cleanup(); }

  T& operator*() {
    CheckValid();
    return *ptr_;
  }
  const T& operator*() const {
    CheckValid();
    return *ptr_;
  }
  T* operator->() {
    CheckValid();
    return ptr_;
  }
  const T* operator->() const {
    CheckValid();
    return ptr_;
  }

  T* ptr() {
    CheckValid();
    return ptr_;
  }

  bool operator==(const SharedPtr<T>& other) {
    CheckValid();
    other.CheckValid();
    return ptr_ == other.ptr_;
  }

  bool empty() { return !init_; }

 private:
  bool init_ = true;
  T* ptr_;
  uint64_t* ref_cnt_;

  void Cleanup() {
    if (!init_) {
      return;
    }
    if (--(*ref_cnt_) == 0) {
      delete ptr_;
      delete ref_cnt_;
    }
  }

  void CheckValid() const {
    if (!init_) {
      panic("Accessing invalid shared ptr");
    }
  }
};

template <typename T, class... A>
SharedPtr<T> MakeShared(A... args) {
  return {new T(args...)};
}
