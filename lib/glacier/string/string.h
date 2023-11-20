#pragma once

#include <stdint.h>

#include "glacier/string/string_view.h"

namespace glcr {

class String {
 public:
  String();
  String(const char* cstr);
  String(const char* cstr, uint64_t str_len);
  String(StringView str);

  String(const String&) = delete;

  const char* cstr() const { return cstr_; }
  uint64_t length() const { return length_; }

  bool operator==(const String& str);

  char operator[](uint64_t offset) const;

  operator StringView() const;
  StringView view() const;

 private:
  char* cstr_;
  uint64_t length_;
};

}  // namespace glcr
