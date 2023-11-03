#include "glacier/string/string_builder.h"

namespace glcr {

void StringBuilder::PushBack(const StringView& str) {
  if (capacity() < size() + str.size()) {
    uint64_t new_capacity = capacity() == 0 ? 1 : capacity() * 2;
    while (new_capacity < size() + str.size()) {
      new_capacity *= 2;
    }
    Resize(new_capacity);
  }
  for (uint64_t i = 0; i < str.size(); i++) {
    Vector<char>::PushBack(str[i]);
  }
}

String StringBuilder::ToString() const { return String(RawPtr(), size()); }

StringBuilder::operator StringView() const {
  return StringView(RawPtr(), size());
}

}  // namespace glcr
