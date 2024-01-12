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

}  // namespace yunq
