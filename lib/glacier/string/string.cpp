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

String::String() : cstr_(nullptr), length_(0) {}

String::String(const char* str) : String(str, cstrlen(str)) {}

String::String(const char* cstr, uint64_t str_len) : length_(str_len) {
  cstr_ = new char[length_ + 1];
  for (uint64_t i = 0; i < length_; i++) {
    cstr_[i] = cstr[i];
  }
  cstr_[length_] = '\0';
}

String::String(StringView str) : String(str.data(), str.size()) {}

String::String(const String& other) : String(other.cstr_, other.length_) {}

String& String::operator=(const String& other) {
  if (cstr_) {
    delete cstr_;
  }
  length_ = other.length_;
  cstr_ = new char[length_ + 1];
  for (uint64_t i = 0; i < length_; i++) {
    cstr_[i] = other.cstr_[i];
  }
  cstr_[length_] = '\0';

  return *this;
}

String::String(String&& other) : cstr_(other.cstr_), length_(other.length_) {
  other.cstr_ = nullptr;
  other.length_ = 0;
}

String& String::operator=(String&& other) {
  if (cstr_) {
    delete cstr_;
  }
  cstr_ = other.cstr_;
  length_ = other.length_;

  other.cstr_ = nullptr;
  other.length_ = 0;

  return *this;
}

String::~String() {
  if (cstr_) {
    delete cstr_;
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

char String::operator[](uint64_t offset) const {
  // FIXME: bounds check.
  return cstr_[offset];
}

String::operator StringView() const { return StringView(cstr_, length_); }
StringView String::view() const { return this->operator StringView(); }

}  // namespace glcr
