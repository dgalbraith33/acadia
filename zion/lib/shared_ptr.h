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
      : ptr_(other.ptr_), ref_cnt_(other.ref_cnt_) {
    (*ref_cnt_)++;
  }

  SharedPtr& operator=(const SharedPtr<T>& other) {
    Cleanup();
    ptr_ = other.ptr_;
    ref_cnt_ = other.ref_cnt_;
    (*ref_cnt_)++;
    return *this;
  }

  ~SharedPtr() { Cleanup(); }

  T& operator*() { return *ptr_; }
  const T& operator*() const { return *ptr_; }
  T* operator->() { return ptr_; }
  const T* operator->() const { return ptr_; }

  T* ptr() { return ptr_; }

  bool operator==(const SharedPtr<T>& other) { return ptr_ == other.ptr_; }

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
      dbgln("Deleting shared ptr: %m", ptr_);
      delete ptr_;
      delete ref_cnt_;
    }
  }
};

template <typename T, class... A>
SharedPtr<T> MakeShared(A... args) {
  return {new T(args...)};
}
