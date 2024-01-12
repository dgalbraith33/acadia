#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/container/optional.h>
#include <glacier/container/vector.h>
#include <glacier/memory/reference.h>
#include <glacier/status/status.h>

#include "yunq/yunq.h"

namespace yunq {

class Serializer {
 public:
  Serializer(glcr::ByteBuffer& bytes, uint64_t offset, uint64_t num_fields)
      : buffer_(bytes),
        offset_(offset),
        next_extension_(kHeaderSize + (8 * num_fields)),
        core_size_(next_extension_),
        caps_() {}
  Serializer(glcr::ByteBuffer& bytes, uint64_t offset, uint64_t num_fields,
             glcr::CapBuffer& caps)
      : buffer_(bytes),
        offset_(offset),
        next_extension_(kHeaderSize + (8 * num_fields)),
        core_size_(next_extension_),
        caps_(caps) {}

  template <typename T>
  void WriteField(uint64_t field_index, const T& value);

  template <typename T>
  void WriteRepeated(uint64_t field_index, const glcr::Vector<T>& value);

  void WriteCapability(uint64_t field_index, uint64_t value);

  void WriteRepeatedCapability(uint64_t field_index,
                               const glcr::Vector<uint64_t>& value);

  template <typename T>
  void WriteMessage(uint64_t field_index, const T& value);

  void WriteHeader();

  uint64_t size() const { return next_extension_; }

 private:
  glcr::ByteBuffer& buffer_;
  uint64_t offset_;
  uint64_t next_extension_;
  uint64_t core_size_;
  uint64_t next_cap_ = 0;
  glcr::Optional<glcr::Ref<glcr::CapBuffer>> caps_;

  uint64_t field_offset(uint64_t field_index) const {
    return offset_ + kHeaderSize + (8 * field_index);
  }
};

template <>
void Serializer::WriteField<uint64_t>(uint64_t field_index,
                                      const uint64_t& value);

template <>
void Serializer::WriteField<glcr::String>(uint64_t field_index,
                                          const glcr::String& value);

template <>
void Serializer::WriteRepeated<uint64_t>(uint64_t field_index,
                                         const glcr::Vector<uint64_t>& value);

template <typename T>
void Serializer::WriteMessage(uint64_t field_index, const T& value) {
  uint64_t length = 0;
  if (caps_) {
    length = value.SerializeToBytes(buffer_, offset_ + next_extension_,
                                    caps_.value().get());
  } else {
    length = value.SerializeToBytes(buffer_, offset_ + next_extension_);
  }

  ExtensionPointer ptr{
      .offset = (uint32_t)next_extension_,
      .length = (uint32_t)length,
  };

  next_extension_ += length;

  buffer_.WriteAt<ExtensionPointer>(field_offset(field_index), ptr);
}

}  // namespace yunq
