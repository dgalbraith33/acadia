#pragma once

#include <glacier/memory/move.h>
#include <stdint.h>

namespace glcr {

template <typename T>
class Vector {
 public:
  // Constructors.
  Vector() : data_(nullptr), size_(0), capacity_(0) {}

  Vector(const Vector&) = delete;
  Vector& operator=(const Vector&) = delete;

  Vector(Vector&& other);
  Vector& operator=(Vector&& other);

  ~Vector() {
    if (data_) {
      delete[] data_;
    }
  }

  // Accessors.
  T& operator[](uint64_t index) { return data_[index]; }
  const T& operator[](uint64_t index) const { return data_[index]; }
  T& at(uint64_t index) { return data_[index]; }
  const T& at(uint64_t index) const { return data_[index]; }

  uint64_t size() const { return size_; }
  bool empty() const { return size_ == 0; }
  uint64_t capacity() const { return capacity_; }

  const T* RawPtr() const { return data_; }

  // Setters.
  // FIXME: Handle downsizing.
  void Resize(uint64_t capacity);

  void PushBack(const T& item);
  void PushBack(T&& item);

  template <typename... Args>
  void EmplaceBack(Args&&... args);

 private:
  T* data_;
  uint64_t size_;
  uint64_t capacity_;

  void Expand();
};  // namespace glcr

template <typename T>
Vector<T>::Vector(Vector&& other)
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;
}

template <typename T>
Vector<T>& Vector<T>::operator=(Vector&& other) {
  if (data_) {
    delete[] data_;
  }

  data_ = other.data_;
  size_ = other.size_;
  capacity_ = other.capacity_;

  other.data_ = nullptr;
  other.size_ = 0;
  other.capacity_ = 0;

  return *this;
}

template <typename T>
void Vector<T>::Resize(uint64_t capacity) {
  T* new_data = new T[capacity];
  if (data_) {
    for (uint64_t i = 0; i < size_; i++) {
      new_data[i] = glcr::Move(data_[i]);
    }
    delete[] data_;
  }
  data_ = new_data;
  capacity_ = capacity;
}

template <typename T>
void Vector<T>::PushBack(const T& item) {
  if (size_ >= capacity_) {
    Expand();
  }

  data_[size_++] = item;
}

template <typename T>
void Vector<T>::PushBack(T&& item) {
  if (size_ >= capacity_) {
    Expand();
  }

  data_[size_++] = glcr::Move(item);
}

template <typename T>
template <typename... Args>
void Vector<T>::EmplaceBack(Args&&... args) {
  if (size_ >= capacity_) {
    Expand();
  }

  data_[size_++] = T(args...);
}

template <typename T>
void Vector<T>::Expand() {
  uint64_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
  Resize(new_capacity);
}

}  // namespace glcr
