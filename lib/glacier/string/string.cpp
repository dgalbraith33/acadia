#include "string/string.h"

namespace glcr {
namespace {

uint64_t cstrlen(const char* cstr) {
  uint64_t len = 0;
  while (*cstr != '\0') {
    cstr++;
    len++;
  }
  return len;
}

}  // namespace

String::String(const char* str) {
  length_ = cstrlen(str);
  cstr_ = new char[length_ + 1];
  for (uint64_t i = 0; i <= length_; i++) {
    cstr_[i] = str[i];
  }
}

bool String::operator==(const String& other) {
  if (other.length_ != length_) {
    return false;
  }
  for (uint64_t i = 0; i < length_; i++) {
    if (cstr_[i] != other.cstr_[i]) {
      return false;
    }
  }
  return true;
}

}  // namespace glcr
