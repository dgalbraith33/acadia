#include "util/hash.h"

#include "string/string.h"

namespace glcr {
namespace {

const uint64_t kFnvOffset = 0xcbf29ce484222325;
const uint64_t kFnvPrime = 0x100000001b3;

}  // namespace

uint64_t Hash<uint64_t>::operator()(const uint64_t& value) {
  uint64_t hash = kFnvOffset;
  for (uint8_t i = 0; i < 8; i++) {
    hash *= kFnvPrime;
    hash ^= (value >> (8 * i)) & 0xFF;
  }
  return hash;
}
uint64_t Hash<String>::operator()(const String& value) {
  uint64_t hash = kFnvOffset;
  for (uint8_t i = 0; i < value.length(); i++) {
    hash *= kFnvPrime;
    hash ^= value[i];
  }
  return hash;
}

}  // namespace glcr
