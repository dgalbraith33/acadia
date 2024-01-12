#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/container/vector.h>
#include <glacier/status/error_or.h>
#include <glacier/status/status.h>

namespace yunq {

const uint64_t kHeaderSize = 24;  // 4x uint32, 1x uint64

struct MessageHeader {
  uint32_t ident;
  uint32_t core_length;
  uint32_t length;
  uint32_t crc32;
  uint64_t options;
} __attribute__((packed));

struct ExtensionPointer {
  uint32_t offset;
  uint32_t length;
};

class MessageView {
 public:
  MessageView(const glcr::ByteBuffer& buffer, uint64_t offset)
      : buffer_(buffer), offset_(offset) {}

  [[nodiscard]] glcr::Status CheckHeader() const;

  // TODO: Implement glcr::StatusOr
  template <typename T>
  glcr::ErrorOr<T> ReadField(uint64_t field_index) const;

  template <typename T>
  glcr::ErrorOr<glcr::Vector<T>> ReadRepeated(uint64_t field_index) const;

 private:
  const glcr::ByteBuffer& buffer_;
  uint64_t offset_;

  uint64_t field_offset(uint64_t field_index) const {
    return offset_ + kHeaderSize + (8 * field_index);
  }
};

template <>
glcr::ErrorOr<uint64_t> MessageView::ReadField<uint64_t>(
    uint64_t field_index) const;

template <>
glcr::ErrorOr<glcr::String> MessageView::ReadField<glcr::String>(
    uint64_t field_index) const;

template <>
glcr::ErrorOr<glcr::Vector<uint64_t>> MessageView::ReadRepeated<uint64_t>(
    uint64_t field_index) const;

}  // namespace yunq
