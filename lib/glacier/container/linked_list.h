#pragma once

#include <stdint.h>

#include "glacier/memory/move.h"

namespace glcr {

template <typename T>
class LinkedList {
 public:
  LinkedList() {}

  LinkedList(const LinkedList&) = delete;
  LinkedList(LinkedList&&) = delete;

  // Accessors.
  bool empty() const { return size_ == 0; }
  uint64_t size() const { return size_; }

  T& PeekFront() { return front_->item; }
  const T& PeekFront() const { return front_->item; }

  T PopFront();

  void PushFront(const T& item);
  void PushFront(T&& item);

  void PushBack(const T& item);
  void PushBack(T&& item);

  struct ListItem {
    T item;
    ListItem* next;
  };
  class Iterator {
   public:
    Iterator(ListItem* item) : item_(item) {}

    Iterator next() { return {item_->next}; }
    Iterator& operator++() {
      item_ = item_->next;
      return *this;
    }

    T& operator*() { return item_->item; }
    T* operator->() { return &item_->item; }
    bool operator==(const Iterator& other) { return item_ == other.item_; }
    bool operator!=(const Iterator& other) { return item_ != other.item_; }

   private:
    ListItem* item_;
  };

  Iterator begin() { return {front_}; }
  Iterator end() { return {nullptr}; }

 private:
  uint64_t size_ = 0;

  ListItem* front_ = nullptr;

  void PushBackInternal(ListItem* new_item) {
    size_++;
    if (front_ == nullptr) {
      front_ = new_item;
      return;
    }
    ListItem* litem = front_;
    while (litem->next != nullptr) {
      litem = litem->next;
    }
    litem->next = new_item;
  }
};

template <typename T>
void LinkedList<T>::PushFront(const T& item) {
  ListItem* new_item = new ListItem{
      .item = item,
      .next = front_,
  };
  front_ = new_item;
  size_++;
}

template <typename T>
void LinkedList<T>::PushFront(T&& item) {
  ListItem* new_item = new ListItem{
      .item = glcr::Move(item),
      .next = front_,
  };
  front_ = new_item;
  size_++;
}

template <typename T>
void LinkedList<T>::PushBack(const T& item) {
  ListItem* new_item = new ListItem{
      .item = item,
      .next = nullptr,
  };
  PushBackInternal(new_item);
}

template <typename T>
void LinkedList<T>::PushBack(T&& item) {
  ListItem* new_item = new ListItem{
      .item = glcr::Move(item),
      .next = nullptr,
  };
  PushBackInternal(new_item);
}

template <typename T>
T LinkedList<T>::PopFront() {
  size_--;

  ListItem* old_front = front_;
  front_ = front_->next;
  T ret = Move(old_front->item);
  delete old_front;
  return Move(ret);
}

}  // namespace glcr
