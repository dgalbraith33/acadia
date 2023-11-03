#include "glacier/string/string_view.h"

namespace glcr {
namespace {

uint64_t cstrlen(const char* ptr) {
  uint64_t len = 0;
  while (ptr[len] != '\0') {
    len++;
  }
  return len;
}

}  // namespace

StringView::StringView() {}
StringView::StringView(const char* str) : value_(str), size_(cstrlen(str)) {}
StringView::StringView(const char* str, uint64_t count)
    : value_(str), size_(count) {}

const char* StringView::data() const { return value_; }

uint64_t StringView::size() const { return size_; }

bool StringView::empty() const { return size_ == 0; }

char StringView::at(uint64_t pos) const { return value_[pos]; }
char StringView::operator[](uint64_t pos) const { return at(pos); };

uint64_t StringView::find(char c, uint64_t pos) const {
  for (uint64_t i = pos; i < size_; i++) {
    if (value_[i] == c) {
      return i;
    }
  }
  return npos;
}

StringView StringView::substr(uint64_t start, uint64_t count) const {
  // FIXME: Report an error here maybe.
  if (start >= size_) {
    return StringView{};
  }
  if (start + count > size_) {
    count = size_ - start;
  }
  return StringView(value_ + start, count);
}

bool operator==(const StringView& str1, const StringView& str2) {
  if (str1.empty() && str2.empty()) {
    return true;
  }
  if (str1.size() != str2.size()) {
    return false;
  }
  if (str1.data() == str2.data()) {
    // Short circuit if they are the same exact string.
    return true;
  }
  for (uint64_t i = 0; i < str1.size(); i++) {
    if (str1.at(i) != str2.at(i)) {
      return false;
    }
  }

  return true;
}

bool operator!=(const StringView& str1, const StringView& str2) {
  return !(str1 == str2);
}

}  // namespace glcr
