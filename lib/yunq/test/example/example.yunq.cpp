// Generated file -- DO NOT MODIFY.
#include "example.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace ex {

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status Basic::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Basic::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  return glcr::Status::Ok();
}

glcr::Status Basic::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse field.
  ASSIGN_OR_RETURN(field_, message.ReadField<uint64_t>(0));

  return glcr::Status::Ok();
}

uint64_t Basic::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  return SerializeInternal(serializer);
}

uint64_t Basic::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  return SerializeInternal(serializer);
}
  
uint64_t Basic::SerializeInternal(yunq::Serializer& serializer) const {
  // Write field.
  serializer.WriteField<uint64_t>(0, field_);

  serializer.WriteHeader();

  return serializer.size();
}


}  // namepace ex
