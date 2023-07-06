#pragma once

#include <stdint.h>

namespace glcr {

class String {
 public:
  String(const char* cstr);
  String(const char* cstr, uint64_t str_len);

  const char* cstr() const { return cstr_; }
  uint64_t length() const { return length_; }

  bool operator==(const String& str);

 private:
  char* cstr_;
  uint64_t length_;
};

}  // namespace glcr
