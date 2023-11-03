#pragma once

#include "glacier/container/vector.h"
#include "glacier/string/string.h"
#include "glacier/string/string_view.h"

namespace glcr {

class StringBuilder : public Vector<char> {
 public:
  StringBuilder() : Vector<char>() {}
  StringBuilder(const StringBuilder&) = delete;
  StringBuilder(StringBuilder&& str) : Vector<char>(Move(str)) {}

  void PushBack(const StringView& str);
  using Vector<char>::PushBack;

  String ToString() const;

  // Note that this could become invalidated
  // at any time if more characters are pushed
  // onto the builder.
  operator StringView() const;
};

}  // namespace glcr
