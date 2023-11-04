#pragma once

namespace glcr {

template <typename T>
class Ref {
 public:
  Ref(T& ref) : ref_(ref) {}
  Ref(const Ref& other) = default;
  Ref(Ref&& other) = default;

  operator T&() const { return ref_; }

 private:
  T& ref_;
};

}  // namespace glcr
