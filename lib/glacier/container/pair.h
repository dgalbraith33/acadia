#pragma once

#include <stdint.h>

#include "glacier/memory/move.h"

namespace glcr {

template <typename T, typename U>
class Pair {
 public:
  Pair(const T& first, const U& second) : first_(first), second_(second) {}
  Pair(T&& first, U&& second) : first_(Move(first)), second_(Move(second)) {}
  T& first() { return first_; }
  U& second() { return second_; }

  bool operator==(const Pair& other) {
    return other.first_ == first_ && other.second_ == second_;
  }

 private:
  T first_;
  U second_;
};

}  // namespace glcr
