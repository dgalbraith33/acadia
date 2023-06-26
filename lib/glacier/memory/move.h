#pragma once

namespace glcr {

template <typename T>
struct RemoveReference {
  typedef T type;
};

template <typename T>
struct RemoveReference<T&> {
  typedef T type;
};

template <typename T>
struct RemoveReference<T&&> {
  typedef T type;
};

template <typename T>
typename RemoveReference<T>::type&& Move(T&& arg) {
  return static_cast<typename RemoveReference<T>::type&&>(arg);
}

}  // namespace glcr
