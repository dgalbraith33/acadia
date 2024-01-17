#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/container/vector.h>
#include <glacier/status/error_or.h>
#include <glacier/status/status.h>

#include "yunq/yunq.h"

namespace yunq {

class MessageView {
 public:
  MessageView(const glcr::ByteBuffer& buffer, uint64_t offset)
      : buffer_(buffer), offset_(offset) {}

  [[nodiscard]] glcr::Status CheckHeader() const;
  uint32_t MessageLength() const;

  // TODO: Implement glcr::StatusOr
  template <typename T>
  glcr::ErrorOr<T> ReadField(uint64_t field_index) const;

  template <typename T>
  glcr::ErrorOr<glcr::Vector<T>> ReadRepeated(uint64_t field_index) const;

  glcr::ErrorOr<uint64_t> ReadCapability(uint64_t field_index) const;
  glcr::ErrorOr<uint64_t> ReadCapability(uint64_t field_index,
                                         const glcr::CapBuffer& caps) const;

  template <typename T>
  glcr::Status ReadMessage(uint64_t field_index, T& message) const;

  template <typename T>
  glcr::Status ReadRepeatedMessage(uint64_t field_index,
                                   glcr::Vector<T>& messages) const;

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
glcr::ErrorOr<int64_t> MessageView::ReadField<int64_t>(
    uint64_t field_index) const;

template <>
glcr::ErrorOr<glcr::String> MessageView::ReadField<glcr::String>(
    uint64_t field_index) const;

template <>
glcr::ErrorOr<glcr::Vector<uint64_t>> MessageView::ReadRepeated<uint64_t>(
    uint64_t field_index) const;

template <typename T>
glcr::Status MessageView::ReadMessage(uint64_t field_index, T& message) const {
  ExtensionPointer ptr =
      buffer_.At<ExtensionPointer>(field_offset(field_index));

  MessageView subview(buffer_, offset_ + ptr.offset);
  return message.ParseFromBytes(subview);
}

template <typename T>
glcr::Status MessageView::ReadRepeatedMessage(uint64_t field_index,
                                              glcr::Vector<T>& messages) const {
  ExtensionPointer ptr =
      buffer_.At<ExtensionPointer>(field_offset(field_index));

  uint64_t ext_offset = ptr.offset;

  while (ext_offset < ptr.offset + ptr.length) {
    MessageView subview(buffer_, offset_ + ext_offset);
    messages.EmplaceBack();
    RETURN_ERROR(messages.PeekBack().ParseFromBytes(subview));
    ext_offset += subview.MessageLength();
  }

  return glcr::Status::Ok();
}

}  // namespace yunq
