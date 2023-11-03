#pragma once

#include <stdint.h>

namespace glcr {

class StringView {
 public:
  static const uint64_t npos = -1;

  StringView();
  StringView(const StringView& other) = default;
  StringView(const char* str);
  StringView(const char* str, uint64_t count);

  const char* data() const;
  uint64_t size() const;

  bool empty() const;

  char at(uint64_t pos) const;
  char operator[](uint64_t pos) const;

  uint64_t find(char c, uint64_t pos = 0) const;

  StringView substr(uint64_t start, uint64_t count) const;

 private:
  const char* value_ = nullptr;
  uint64_t size_ = 0;
};

bool operator==(const StringView& str1, const StringView& str2);
bool operator!=(const StringView& str1, const StringView& str2);

}  // namespace glcr
