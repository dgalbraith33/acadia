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

}  // namespace glcr
