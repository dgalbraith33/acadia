#pragma once

#include <glacier/memory/move.h>
#include <stdint.h>

namespace glcr {

template <typename T>
class Vector {
 public:
  Vector() : data_(nullptr), size_(0), capacity_(0) {}

  Vector(const Vector&) = delete;
  Vector(Vector&& other)
      : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = 0;
    other.capacity_ = 0;
  }
  Vector& operator=(Vector&& other) {
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

  ~Vector() {
    if (data_) {
      delete[] data_;
    }
  }

  // FIXME: Handle downsizing.
  void Resize(uint64_t capacity);

  // Setters.
  void PushBack(const T& item);
  void PushBack(T&& item);
  template <typename... Args>
  void EmplaceBack(Args... args);

  // Accessors.
  T& operator[](uint64_t index);
  const T& operator[](uint64_t index) const;
  T& at(uint64_t index);
  const T& at(uint64_t index) const;

  uint64_t size() const { return size_; }
  uint64_t capacity() const { return capacity_; }

  const T* RawPtr() const { return data_; }

 private:
  T* data_;
  uint64_t size_;
  uint64_t capacity_;

  void Expand();
};

template <typename T>
void Vector<T>::Resize(uint64_t capacity) {
  T* new_data = reinterpret_cast<T*>(new uint8_t[capacity * sizeof(T)]);
  for (uint64_t i = 0; i < size_; i++) {
    new_data[i] = glcr::Move(data_[i]);
  }
  delete[] data_;
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
void Vector<T>::EmplaceBack(Args... args) {
  if (size_ >= capacity_) {
    Expand();
  }

  data_[size_++] = T(args...);
}

template <typename T>
T& Vector<T>::operator[](uint64_t index) {
  return data_[index];
}

template <typename T>
const T& Vector<T>::operator[](uint64_t index) const {
  return data_[index];
}

template <typename T>
T& Vector<T>::at(uint64_t index) {
  return data_[index];
}

template <typename T>
const T& Vector<T>::at(uint64_t index) const {
  return data_[index];
}

template <typename T>
void Vector<T>::Expand() {
  uint64_t new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
  Resize(new_capacity);
}

}  // namespace glcr
