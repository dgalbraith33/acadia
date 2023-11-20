#pragma once

#include "glacier/memory/move.h"
#include "glacier/status/error.h"
#include "glacier/string/string_builder.h"
#include "glacier/string/string_view.h"

namespace glcr {

// FIXME: We need some meta-programming here to allow pass-by-value for pointers
// and primitives.
template <typename T>
void StrFormatValue(StringBuilder& builder, const T& value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const uint8_t& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const uint16_t& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const int32_t& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const uint32_t& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const uint64_t& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const ErrorCode& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const char& value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, char const* const& value,
                    StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const StringView& value,
                    StringView opts);

void StrFormatInternal(StringBuilder& builder, StringView format);

template <typename T, typename... Args>
void StrFormatInternal(StringBuilder& builder, StringView format,
                       const T& value, Args&&... args) {
  uint64_t posl = format.find('{');
  uint64_t posr = format.find('}', posl);
  if (posl == format.npos || posr == format.npos) {
    // TODO: Consider throwing an error since we still have values to consume.
    return StrFormatInternal(builder, format);
  }

  builder.PushBack(format.substr(0, posl));
  StrFormatValue(builder, value, format.substr(posl + 1, posr - posl - 1));
  StrFormatInternal(builder, format.substr(posr + 1, format.size() - posr - 1),
                    args...);
}

template <typename... Args>
String StrFormat(StringView format, Args&&... args) {
  VariableStringBuilder builder;
  StrFormatInternal(builder, format, args...);
  return builder.ToString();
}

template <typename... Args>
void StrFormatIntoBuffer(StringBuilder& builder, StringView format,
                         Args&&... args) {
  StrFormatInternal(builder, format, args...);
}

}  // namespace glcr
