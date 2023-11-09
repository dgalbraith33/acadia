#include "glacier/string/string_builder.h"

namespace glcr {

uint64_t VariableStringBuilder::size() const { return data_.size(); }

void VariableStringBuilder::PushBack(const StringView& str) {
  if (data_.capacity() < size() + str.size()) {
    uint64_t new_capacity = data_.capacity() == 0 ? 1 : data_.capacity() * 2;
    while (new_capacity < size() + str.size()) {
      new_capacity *= 2;
    }
    data_.Resize(new_capacity);
  }
  for (uint64_t i = 0; i < str.size(); i++) {
    data_.PushBack(str[i]);
  }
}

void VariableStringBuilder::PushBack(const char str) { data_.PushBack(str); }

String VariableStringBuilder::ToString() const {
  return String(data_.RawPtr(), size());
}

VariableStringBuilder::operator StringView() const {
  return StringView(data_.RawPtr(), size());
}

void FixedStringBuilder::PushBack(const StringView& str) {
  for (uint64_t i = 0; i < str.size(); i++) {
    PushBack(str[i]);
  }
}

void FixedStringBuilder::PushBack(const char str) {
  if (size_ >= capacity_) {
    // Somewhat random sequence of characters to show that we've overrun the
    // buffer.
    buffer_[capacity_ - 1] = '>';
    buffer_[capacity_ - 2] = '!';
  } else {
    buffer_[size_++] = str;
  }
}

String FixedStringBuilder::ToString() const { return String(buffer_, size_); }

FixedStringBuilder::operator StringView() const {
  return StringView(buffer_, size_);
}

}  // namespace glcr
