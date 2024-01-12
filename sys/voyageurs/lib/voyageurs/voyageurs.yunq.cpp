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
glcr::Status KeyboardListener::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse port_capability.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_port_capability(0);
  return glcr::Status::Ok();
}

glcr::Status KeyboardListener::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  yunq::MessageView message(bytes, offset);
  RETURN_ERROR(ParseFromBytesInternal(message));
  // Parse port_capability.
  uint64_t port_capability_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_port_capability(caps.At(port_capability_ptr));
  return glcr::Status::Ok();
}

glcr::Status KeyboardListener::ParseFromBytesInternal(const yunq::MessageView& message) {
  RETURN_ERROR(message.CheckHeader());
  // Parse port_capability.

  return glcr::Status::Ok();
}

uint64_t KeyboardListener::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write port_capability.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), 0);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t KeyboardListener::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;
  // Write port_capability.
  caps.WriteAt(next_cap, port_capability());
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), next_cap++);

  // The next extension pointer is the length of the message. 
  yunq::WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

