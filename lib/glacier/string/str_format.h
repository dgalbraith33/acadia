#pragma once

#include "glacier/memory/move.h"
#include "glacier/status/error.h"
#include "glacier/string/string_builder.h"
#include "glacier/string/string_view.h"

namespace glcr {

template <typename T>
void StrFormatValue(StringBuilder& builder, T value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, uint8_t value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, uint16_t value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, int32_t value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, uint32_t value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, uint64_t value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, ErrorCode value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, char value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, const char* value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, StringView value, StringView opts);

template <>
void StrFormatValue(StringBuilder& builder, String value, StringView opts);

void StrFormatInternal(StringBuilder& builder, StringView format);

template <typename T, typename... Args>
void StrFormatInternal(StringBuilder& builder, StringView format, T value,
                       Args... args) {
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
String StrFormat(StringView format, Args... args) {
  VariableStringBuilder builder;
  StrFormatInternal(builder, format, args...);
  return builder.ToString();
}

template <typename... Args>
void StrFormat(StringBuilder& builder, StringView format, Args... args) {
  StrFormatInternal(builder, format, args...);
}

}  // namespace glcr
