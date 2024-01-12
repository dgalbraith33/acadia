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

  // TODO: Implement glcr::StatusOr
  template <typename T>
  glcr::ErrorOr<T> ReadField(uint64_t field_index) const;

  template <typename T>
  glcr::ErrorOr<glcr::Vector<T>> ReadRepeated(uint64_t field_index) const;

  glcr::ErrorOr<uint64_t> ReadCapability(uint64_t field_index) const;
  glcr::ErrorOr<uint64_t> ReadCapability(uint64_t field_index,
                                         const glcr::CapBuffer& caps) const;

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
