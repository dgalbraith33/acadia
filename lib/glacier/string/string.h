#pragma once

#include <stdint.h>

namespace glcr {

class String {
 public:
  String(const char* cstr);

  bool operator==(const String& str);

 private:
  char* cstr_;
  uint64_t length_;
};

}  // namespace glcr
