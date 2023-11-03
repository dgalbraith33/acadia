#pragma once

#include "glacier/container/vector.h"
#include "glacier/string/string_view.h"

namespace glcr {

// TODO: Add a split that uses a StringView as a delimeter.
Vector<StringView> StrSplit(const StringView& str, char delimeter);

}  // namespace glcr
