#include "message_view.h"

namespace yunq {

namespace {

const uint64_t kIdentByte = 0x33441122;

}  // namespace

glcr::Status MessageView::CheckHeader() const {
  if (buffer_.At<uint32_t>(offset_ + 0) != kIdentByte) {
    return glcr::InvalidArgument("Trying to parse an invalid yunq message.");
  }
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
  return glcr::Status::Ok();
}

template <>
glcr::ErrorOr<uint64_t> MessageView::ReadField<uint64_t>(
    uint64_t field_index) const {
  return buffer_.At<uint64_t>(field_offset(field_index));
}

template <>
glcr::ErrorOr<glcr::String> MessageView::ReadField<glcr::String>(
    uint64_t field_index) const {
  ExtensionPointer ptr =
      buffer_.At<ExtensionPointer>(field_offset(field_index));

  return buffer_.StringAt(offset_ + ptr.offset, ptr.length);
}

template <>
glcr::ErrorOr<glcr::Vector<uint64_t>> MessageView::ReadRepeated<uint64_t>(
    uint64_t field_index) const {
  ExtensionPointer pointer =
      buffer_.At<ExtensionPointer>(field_offset(field_index));

  glcr::Vector<uint64_t> v;
  v.Resize(pointer.length / sizeof(uint64_t));
  for (uint64_t i = offset_ + pointer.offset;
       i < offset_ + pointer.offset + pointer.length; i += sizeof(uint64_t)) {
    v.PushBack(buffer_.At<uint64_t>(i));
  }
  return v;
}

glcr::ErrorOr<uint64_t> MessageView::ReadCapability(
    uint64_t field_index) const {
  return buffer_.At<uint64_t>(field_offset(field_index));
}

glcr::ErrorOr<uint64_t> MessageView::ReadCapability(
    uint64_t field_index, const glcr::CapBuffer& caps) const {
  uint64_t offset = buffer_.At<uint64_t>(field_offset(field_index));
  return caps.At(offset);
}
}  // namespace yunq
