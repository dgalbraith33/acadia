#pragma once

namespace glcr {

template <typename T>
class UniquePtr {
 public:
  UniquePtr() : ptr_(nullptr) {}
  UniquePtr(decltype(nullptr)) : ptr_(nullptr) {}
  UniquePtr(T* ptr) : ptr_(ptr) {}

  UniquePtr(const UniquePtr<T>&) = delete;
  UniquePtr& operator=(const UniquePtr<T>&) = delete;

  UniquePtr(UniquePtr<T>&& other) : ptr_(other.ptr_) { other.ptr_ = nullptr; }
  UniquePtr& operator=(UniquePtr<T>&& other) {
    T* temp = ptr_;
    ptr_ = other.ptr_;
    other.ptr_ = temp;
  }

  ~UniquePtr() {
    if (ptr_ != nullptr) {
      delete ptr_;
    }
  }

  explicit operator bool() const { return ptr_ != nullptr; }
  bool empty() const { return ptr_ == nullptr; }

  T* get() const { return ptr_; }

  T* operator->() const { return ptr_; }
  T& operator*() const { return *ptr_; }

  T* release() {
    T* tmp = ptr_;
    ptr_ = nullptr;
    return tmp;
  }

 private:
  T* ptr_;
};

template <typename T, typename... Args>
UniquePtr<T> MakeUnique(Args... args) {
  return UniquePtr(new T(args...));
}

}  // namespace glcr
