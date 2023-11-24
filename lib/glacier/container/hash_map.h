#pragma once

#include <stdint.h>

#include "glacier/container/array.h"
#include "glacier/container/linked_list.h"
#include "glacier/container/pair.h"
#include "glacier/status/error.h"
#include "glacier/string/string.h"
#include "glacier/util/hash.h"

namespace glcr {

template <typename K, typename V, class H = Hash<K>>
class HashMap {
 public:
  HashMap() = default;
  HashMap(const HashMap&) = delete;
  HashMap& operator=(const HashMap&) = delete;
  // TODO: Implement Move.
  HashMap(HashMap&&) = delete;
  HashMap& operator=(HashMap&&) = delete;

  // Accessors.
  uint64_t size() { return size_; }
  uint64_t empty() { return size_ == 0; }

  // Returns load as a percentage (i.e. 60 means the load is 0.6).
  //
  // If data is a zero-size array, return load as 100 so it will be flagged for
  // resize.
  // TODO: Return a double here once FPE is enabled.
  uint64_t load() {
    if (data_.size() == 0) {
      return 100;
    }
    return size_ * 100 / data_.size();
  }

  V& at(const K&);
  const V& at(const K&) const;

  bool Contains(const K&) const;

  // Setters.
  [[nodiscard]] ErrorCode Insert(const K&, const V&);
  [[nodiscard]] ErrorCode Insert(K&&, V&&);

  [[nodiscard]] ErrorCode Update(const K&, const V&);
  [[nodiscard]] ErrorCode Update(const K&, V&&);

  [[nodiscard]] ErrorCode Delete(const K&);

  void Resize(uint64_t new_size);

 private:
  Array<LinkedList<Pair<K, V>>> data_;
  uint64_t size_ = 0;

  void ResizeIfNecessary();
};

template <typename K, typename V, class H>
V& HashMap<K, V, H>::at(const K& key) {
  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      return pair.second();
    }
  }
  // TODO: Add a failure mode here instead of constructing an object.
  ll.PushFront({key, {}});
  return ll.PeekFront().second();
}

template <typename K, typename V, class H>
const V& HashMap<K, V, H>::at(const K& key) const {
  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      return pair.second();
    }
  }
  // TODO: Add a failure mode here instead of constructing an object.
  ll.PushFront({key, {}});
  return ll.PeekFront().second();
}

template <typename K, typename V, class H>
bool HashMap<K, V, H>::Contains(const K& key) const {
  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      return true;
    }
  }
  return false;
}

template <typename K, typename V, class H>
ErrorCode HashMap<K, V, H>::Insert(const K& key, const V& value) {
  ResizeIfNecessary();

  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      return ALREADY_EXISTS;
    }
  }
  ll.PushFront({key, value});
  size_++;
  return OK;
}

template <typename K, typename V, class H>
ErrorCode HashMap<K, V, H>::Insert(K&& key, V&& value) {
  ResizeIfNecessary();

  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      return ALREADY_EXISTS;
    }
  }
  ll.PushFront({Move(key), Move(value)});
  size_++;
  return OK;
}

template <typename K, typename V, class H>
ErrorCode HashMap<K, V, H>::Update(const K& key, const V& value) {
  ResizeIfNecessary();

  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      pair.second() = value;
      return OK;
    }
  }
  return NOT_FOUND;
}

template <typename K, typename V, class H>
ErrorCode HashMap<K, V, H>::Update(const K& key, V&& value) {
  ResizeIfNecessary();

  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      pair.second() = Move(value);
      return OK;
    }
  }
  return NOT_FOUND;
}

template <typename K, typename V, class H>
ErrorCode HashMap<K, V, H>::Delete(const K& key) {
  uint64_t hc = H()(key);
  auto& ll = data_[hc % data_.size()];

  for (auto& pair : ll) {
    if (pair.first() == key) {
      ll.Remove(pair);
      size_--;
      return OK;
    }
  }
  return NOT_FOUND;
}

template <typename K, typename V, class H>
void HashMap<K, V, H>::Resize(uint64_t new_size) {
  Array<LinkedList<Pair<K, V>>> new_data(new_size);

  for (uint64_t i = 0; i < data_.size(); i++) {
    auto& ll = data_[i];
    while (!ll.empty()) {
      auto pair = ll.PopFront();
      uint64_t hc = H()(pair.first());
      new_data[hc % new_size].PushFront(Move(pair));
    }
  }
  data_ = glcr::Move(new_data);
}

template <typename K, typename V, class H>
void HashMap<K, V, H>::ResizeIfNecessary() {
  if (data_.size() == 0) {
    Resize(8);
  } else if (load() > 75) {
    Resize(data_.size() * 2);
  }
}

}  // namespace glcr
