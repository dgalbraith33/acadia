#include "glacier/string/str_format.h"

namespace glcr {

namespace {

void StrFormatNumber(StringBuilder& builder, uint64_t value, uint64_t base) {
  const char* kHexCharacters = "0123456789ABCDEF";

  if (value < base) {
    builder.PushBack(kHexCharacters[value]);
    return;
  }

  StrFormatNumber(builder, value / base, base);

  builder.PushBack(kHexCharacters[value % base]);
}

}  // namespace

template <>
void StrFormatValue(StringBuilder& builder, const uint8_t& value,
                    StringView opts) {
  StrFormatValue(builder, static_cast<uint64_t>(value), opts);
}

template <>
void StrFormatValue(StringBuilder& builder, const uint16_t& value,
                    StringView opts) {
  StrFormatValue(builder, static_cast<uint64_t>(value), opts);
}

template <>
void StrFormatValue(StringBuilder& builder, const int32_t& value,
                    StringView opts) {
  StrFormatValue(builder, static_cast<uint64_t>(value), opts);
}

template <>
void StrFormatValue(StringBuilder& builder, const uint32_t& value,
                    StringView opts) {
  StrFormatValue(builder, static_cast<uint64_t>(value), opts);
}

template <>
void StrFormatValue(StringBuilder& builder, const uint64_t& value,
                    StringView opts) {
  if (opts.find('x') != opts.npos) {
    builder.PushBack("0x");
    StrFormatNumber(builder, value, 16);
  } else {
    StrFormatNumber(builder, value, 10);
  }
}

template <>
void StrFormatValue(StringBuilder& builder, const ErrorCode& value,
                    StringView opts) {
  StrFormatValue(builder, ErrorCodeToStr(value), opts);
}

template <>
void StrFormatValue(StringBuilder& builder, const char& value,
                    StringView opts) {
  builder.PushBack(value);
}

template <>
void StrFormatValue(StringBuilder& builder, char const* const& value,
                    StringView opts) {
  StrFormatInternal(builder, StringView(value));
}

template <>
void StrFormatValue(StringBuilder& builder, const StringView& value,
                    StringView opts) {
  StrFormatInternal(builder, value);
}

void StrFormatInternal(StringBuilder& builder, StringView format) {
  // TODO: Consider throwing an error if there are unhandled format
  builder.PushBack(format);
}

}  // namespace glcr
