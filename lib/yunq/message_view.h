#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/status/error_or.h>

namespace yunq {

const uint64_t kHeaderSize = 24;  // 4x uint32, 1x uint64

struct ExtensionPointer {
  uint32_t offset;
  uint32_t length;
};

class MessageView {
 public:
  MessageView(const glcr::ByteBuffer& buffer, uint64_t offset)
      : buffer_(buffer), offset_(offset) {}

  // TODO: Implement glcr::StatusOr
  template <typename T>
  glcr::ErrorOr<T> ReadField(uint64_t field_index);

 private:
  const glcr::ByteBuffer& buffer_;
  uint64_t offset_;

  uint64_t field_offset(uint64_t field_index) {
    return offset_ + kHeaderSize + (8 * field_index);
  }
};

template <>
glcr::ErrorOr<uint64_t> MessageView::ReadField<uint64_t>(uint64_t field_index);

template <>
glcr::ErrorOr<glcr::String> MessageView::ReadField<glcr::String>(
    uint64_t field_index);

}  // namespace yunq
