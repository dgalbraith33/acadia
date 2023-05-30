#pragma once

#include <stdint.h>

#include "debug/debug.h"

template <typename T>
class LinkedList {
 public:
  LinkedList() {}

  LinkedList(const LinkedList&) = delete;

  uint64_t size() const { return size_; }

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

  /*
   * Returns the front item in the list and pushes the passed item to the back.
   *
   * Done in one function to avoid a memory alloc/dealloc during scheduling.
   **/
  T CycleFront(const T& new_item) {
    if (size_ == 0 || front_ == nullptr) {
      panic("Cycling empty list");
    }

    T ret = front_->item;
    front_->item = new_item;
    if (size_ == 1) {
      return ret;
    }

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

  T PeekFront() const { return front_->item; }

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
    T& operator->() { return item_->item; }
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
};
