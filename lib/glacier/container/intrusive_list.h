#pragma once

#include <glacier/memory/ref_ptr.h>
#include <stdint.h>

namespace glcr {

template <typename T>
class IntrusiveListNode {
 public:
  RefPtr<T> next_;
  RefPtr<T> prev_;
};

template <typename T>
class IntrusiveList {
 public:
  IntrusiveList() : front_(nullptr), back_(nullptr) {}

  void PushFront(const RefPtr<T>& obj);
  void PushBack(const RefPtr<T>& obj);

  void Remove(const RefPtr<T>& obj);

  RefPtr<T> PopFront();
  RefPtr<T> PopBack();

  RefPtr<T> PeekFront() { return front_; }
  RefPtr<T> PeekBack() { return back_; }

  uint64_t size() { return size_; }

 private:
  RefPtr<T> front_;
  RefPtr<T> back_;

  uint64_t size_ = 0;
};

template <typename T>
void IntrusiveList<T>::PushFront(const RefPtr<T>& obj) {
  // FIXME: Check to make sure obj next and prev aren't set.
  size_++;
  obj->next_ = front_;
  obj->prev_ = nullptr;
  if (front_ == nullptr) {
    front_ = obj;
    back_ = obj;
    return;
  }

  front_->prev_ = obj;
  front_ = obj;
}

template <typename T>
void IntrusiveList<T>::PushBack(const RefPtr<T>& obj) {
  if (front_ == nullptr) {
    PushFront(obj);
    return;
  }
  size_++;
  back_->next_ = obj;
  obj->prev_ = back_;
  obj->next_ = nullptr;
  back_ = obj;
}

template <typename T>
void IntrusiveList<T>::Remove(const RefPtr<T>& obj) {
  if (!obj) {
    return;
  }
  if (front_ == obj) {
    front_ = obj->next_;
  }
  if (back_ == obj) {
    back_ = obj->prev_;
  }
  if (obj->prev_) {
    obj->prev_->next_ = obj->next_;
  }
  if (obj->next_) {
    obj->next_->prev_ = obj->prev_;
  }

  obj->prev_ = nullptr;
  obj->next_ = nullptr;
  size_--;
}

template <typename T>
RefPtr<T> IntrusiveList<T>::PopFront() {
  if (front_ == nullptr) {
    return nullptr;
  }
  size_--;
  if (front_ == back_) {
    RefPtr<T> ptr = front_;
    front_ = nullptr;
    back_ = nullptr;
    return ptr;
  }
  RefPtr<T> ptr = front_;
  front_ = front_->next_;
  front_->prev_ = nullptr;
  ptr->next_ = nullptr;
  return ptr;
}

template <typename T>
RefPtr<T> IntrusiveList<T>::PopBack() {
  if (back_ == nullptr) {
    return nullptr;
  }
  if (front_ == back_) {
    return PopFront();
  }
  size_--;
  RefPtr<T> ptr = back_;
  back_ = back_->prev_;
  back_->next_ = nullptr;
  ptr->prev_ = nullptr;
  return ptr;
}

}  // namespace glcr
