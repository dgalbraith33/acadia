#include "serialize.h"

namespace yunq {
namespace {

const uint64_t kIdentByte = 0x33441122;

}  // namespace

void Serializer::WriteHeader() {
  buffer_.WriteAt<uint32_t>(offset_ + 0, kIdentByte);
  buffer_.WriteAt<uint32_t>(offset_ + 4, core_size_);
  buffer_.WriteAt<uint32_t>(offset_ + 8, next_extension_);
  buffer_.WriteAt<uint32_t>(offset_ + 12, 0);  // TODO: Calculate CRC32.
}

template <>
void Serializer::WriteField<uint64_t>(uint64_t field_index,
                                      const uint64_t& value) {
  buffer_.WriteAt<uint64_t>(field_offset(field_index), value);
}

template <>
void Serializer::WriteField<int64_t>(uint64_t field_index,
                                     const int64_t& value) {
  buffer_.WriteAt<int64_t>(field_offset(field_index), value);
}

template <>
void Serializer::WriteField<glcr::String>(uint64_t field_index,
                                          const glcr::String& value) {
  ExtensionPointer ptr{
      .offset = (uint32_t)next_extension_,
      // FIXME: Check downcast of str length.
      .length = (uint32_t)value.length(),
  };

  buffer_.WriteStringAt(offset_ + next_extension_, value);
  next_extension_ += ptr.length;

  buffer_.WriteAt<ExtensionPointer>(field_offset(field_index), ptr);
}

template <>
void Serializer::WriteRepeated<uint64_t>(uint64_t field_index,
                                         const glcr::Vector<uint64_t>& value) {
  ExtensionPointer ptr{
      .offset = (uint32_t)next_extension_,
      .length = (uint32_t)(value.size() * sizeof(uint64_t)),
  };

  next_extension_ += ptr.length;
  buffer_.WriteAt<ExtensionPointer>(field_offset(field_index), ptr);

  for (uint64_t i = 0; i < value.size(); i++) {
    uint32_t ext_offset = offset_ + ptr.offset + (i * sizeof(uint64_t));
    buffer_.WriteAt<uint64_t>(ext_offset, value.at(i));
  }
}

void Serializer::WriteCapability(uint64_t field_index, uint64_t value) {
  if (caps_) {
    buffer_.WriteAt<uint64_t>(field_offset(field_index), next_cap_);
    caps_.value().get().WriteAt(next_cap_++, value);
  } else {
    WriteField<uint64_t>(field_index, value);
  }
}

void Serializer::WriteRepeatedCapability(uint64_t field_index,
                                         const glcr::Vector<uint64_t>& value) {}

}  // namespace yunq
