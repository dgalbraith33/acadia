#pragma once

namespace glcr {

template <typename T, typename U>
class Pair {
 public:
  Pair(const T& first, const U& second) : first_(first), second_(second) {}
  T& first() { return first_; }
  U& second() { return second_; }

 private:
  T first_;
  U second_;
};

}  // namespace glcr
