#include "message_view.h"

namespace yunq {

template <>
glcr::ErrorOr<uint64_t> MessageView::ReadField<uint64_t>(uint64_t field_index) {
  return buffer_.At<uint64_t>(field_offset(field_index));
}

template <>
glcr::ErrorOr<glcr::String> MessageView::ReadField<glcr::String>(
    uint64_t field_index) {
  ExtensionPointer ptr =
      buffer_.At<ExtensionPointer>(field_offset(field_index));

  return buffer_.StringAt(offset_ + ptr.offset, ptr.length);
}

template <>
glcr::ErrorOr<glcr::Vector<uint64_t>> MessageView::ReadRepeated<uint64_t>(
    uint64_t field_index) {
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

}  // namespace yunq
