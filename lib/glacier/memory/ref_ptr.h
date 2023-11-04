#pragma once

namespace glcr {

template <typename T>
class RefPtr;

template <typename T>
RefPtr<T> AdoptPtr(T* ptr);

template <typename T, typename U>
RefPtr<T> StaticCastRefPtr(const RefPtr<U>& ref);

template <typename T>
class RefPtr {
 public:
  RefPtr() : ptr_(nullptr) {}
  RefPtr(decltype(nullptr)) : ptr_(nullptr) {}
  RefPtr(const RefPtr& other) : ptr_(other.ptr_) {
    if (ptr_) {
      ptr_->Acquire();
    }
  }
  RefPtr& operator=(const RefPtr& other) {
    T* old = ptr_;
    ptr_ = other.ptr_;
    if (ptr_) {
      ptr_->Acquire();
    }
    if (old && old->Release()) {
      delete old;
    }

    return *this;
  }

  RefPtr(RefPtr&& other) : ptr_(other.ptr_) { other.ptr_ = nullptr; }
  RefPtr& operator=(RefPtr&& other) {
    // Swap
    T* ptr = ptr_;
    ptr_ = other.ptr_;
    other.ptr_ = ptr;
    return *this;
  }

  enum DontAdoptTag {
    DontAdopt,
  };
  RefPtr(T* ptr, DontAdoptTag) : ptr_(ptr) { ptr->Acquire(); }

  T* get() const { return ptr_; };
  T& operator*() const { return *ptr_; }
  T* operator->() const { return ptr_; }

  bool empty() const { return ptr_ == nullptr; }
  operator bool() const { return ptr_ != nullptr; }

  bool operator==(decltype(nullptr)) const { return (ptr_ == nullptr); }
  bool operator!=(decltype(nullptr)) const { return (ptr_ != nullptr); }

  bool operator==(const RefPtr<T>& other) const { return (ptr_ == other.ptr_); }
  bool operator!=(const RefPtr<T>& other) const { return (ptr_ != other.ptr_); }

 private:
  T* ptr_;

  friend RefPtr<T> AdoptPtr<T>(T* ptr);
  RefPtr(T* ptr) : ptr_(ptr) { ptr->Adopt(); }
};

template <typename T>
class MakeRefCountedFriend final {
 public:
  template <typename... Args>
  static RefPtr<T> Make(Args&&... args) {
    return AdoptPtr(new T(args...));
  }
};

template <typename T, typename... Args>
RefPtr<T> MakeRefCounted(Args&&... args) {
  return MakeRefCountedFriend<T>::Make(args...);
}

template <typename T>
RefPtr<T> AdoptPtr(T* ptr) {
  return RefPtr(ptr);
}

template <typename T, typename U>
RefPtr<T> StaticCastRefPtr(const RefPtr<U>& ref) {
  return RefPtr(static_cast<T*>(ref.get()), RefPtr<T>::DontAdopt);
}

}  // namespace glcr
