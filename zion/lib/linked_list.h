#pragma once

#include <stdint.h>

#include "debug/debug.h"

template <typename T>
class LinkedList {
 public:
  LinkedList() {}

  LinkedList(const LinkedList&) = delete;

  uint64_t size() { return size_; }

  void PushBack(const T& item) {
    size_++;
    ListItem* new_item = new ListItem{
        .item = item,
        .next = nullptr,
    };
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

  T PopFront() {
    if (size_ == 0 || front_ == nullptr) {
      panic("Popping from empty list");
    }

    size_--;

    ListItem* old_front = front_;
    front_ = front_->next;
    T ret = old_front->item;
    delete old_front;
    return ret;
  }

  T CycleFront() {
    if (size_ == 0 || front_ == nullptr) {
      panic("Cycling empty list");
    }

    if (size_ == 1) {
      return front_->item;
    }

    T ret = front_->item;
    ListItem* old_front = front_;
    ListItem* iter = front_;
    front_ = front_->next;
    while (iter->next != nullptr) {
      iter = iter->next;
    }
    iter->next = old_front;
    old_front->next = nullptr;
    return ret;
  }

  T PeekFront() { return front_->item; }

 private:
  uint64_t size_ = 0;

  struct ListItem {
    T item;
    ListItem* next;
  };

  ListItem* front_ = nullptr;
};
