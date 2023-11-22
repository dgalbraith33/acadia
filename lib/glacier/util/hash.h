#pragma once

#include <stdint.h>

namespace glcr {

// General purpose templated hash function.
// Currently the template speciializations
// implement FNV hashing:
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
template <typename T>
struct Hash {
  uint64_t operator()(const T&);
};

template <>
struct Hash<uint64_t> {
  uint64_t operator()(const uint64_t&);
};

class String;
template <>
struct Hash<String> {
  uint64_t operator()(const String&);
};

}  // namespace glcr
