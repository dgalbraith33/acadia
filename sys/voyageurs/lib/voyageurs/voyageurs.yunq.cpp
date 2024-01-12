// Generated file -- DO NOT MODIFY.
#include "voyageurs.yunq.h"

#include <yunq/message_view.h>
#include <yunq/serialize.h>


namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

}  // namespace
glcr::Status KeyboardListener::ParseFromBytes(const yunq::MessageView& message) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse port_capability.
  ASSIGN_OR_RETURN(port_capability_, message.ReadCapability(0));
  return glcr::Status::Ok();
}

glcr::Status KeyboardListener::ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer& caps) {
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse port_capability.
  ASSIGN_OR_RETURN(port_capability_, message.ReadCapability(0, caps));
  return glcr::Status::Ok();
}

glcr::Status KeyboardListener::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse port_capability.

  return glcr::Status::Ok();
}

uint64_t KeyboardListener::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  yunq::Serializer serializer(bytes, offset, 1);
  // Write port_capability.
  serializer.WriteCapability(0, port_capability_);

  serializer.WriteHeader();

  return serializer.size();
}

uint64_t KeyboardListener::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  yunq::Serializer serializer(bytes, offset, 1, caps);
  // Write port_capability.
  serializer.WriteCapability(0, port_capability_);

  serializer.WriteHeader();

  return serializer.size();
}

