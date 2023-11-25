// Generated file -- DO NOT MODIFY.
#include "voyageurs.yunq.h"

namespace {

const uint64_t header_size = 24;  // 4x uint32, 1x uint64

struct ExtPointer {
  uint32_t offset;
  uint32_t length;
};

void CheckHeader(const glcr::ByteBuffer& bytes) {
  // TODO: Check ident.
  // TODO: Parse core size.
  // TODO: Parse extension size.
  // TODO: Check CRC32
  // TODO: Parse options.
}

void WriteHeader(glcr::ByteBuffer& bytes, uint64_t offset, uint32_t core_size, uint32_t extension_size) {
  bytes.WriteAt<uint32_t>(offset + 0, 0xDEADBEEF);  // TODO: Chose a more unique ident sequence.
  bytes.WriteAt<uint32_t>(offset + 4, core_size);
  bytes.WriteAt<uint32_t>(offset + 8, extension_size);
  bytes.WriteAt<uint32_t>(offset + 12, 0); // TODO: Calculate CRC32.
  bytes.WriteAt<uint64_t>(offset + 16, 0); // TODO: Add options.
}

}  // namespace
void KeyboardListener::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  ParseFromBytesInternal(bytes, offset);
  // Parse port_capability.
  // FIXME: Implement in-buffer capabilities for inprocess serialization.
  set_port_capability(0);
}

void KeyboardListener::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  ParseFromBytesInternal(bytes, offset);
  // Parse port_capability.
  uint64_t port_capability_ptr = bytes.At<uint64_t>(offset + header_size + (8 * 0));

  set_port_capability(caps.At(port_capability_ptr));
}

void KeyboardListener::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);
  // Parse port_capability.
  // Skip Cap.

}

uint64_t KeyboardListener::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 1;
  const uint32_t core_size = next_extension;
  // Write port_capability.
  // FIXME: Implement inbuffer capabilities.
  bytes.WriteAt<uint64_t>(offset + header_size + (8 * 0), 0);

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

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
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}
void None::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset) {
  ParseFromBytesInternal(bytes, offset);
}

void None::ParseFromBytes(const glcr::ByteBuffer& bytes, uint64_t offset, const glcr::CapBuffer& caps) {
  ParseFromBytesInternal(bytes, offset);
}

void None::ParseFromBytesInternal(const glcr::ByteBuffer& bytes, uint64_t offset) {
  CheckHeader(bytes);

}

uint64_t None::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset) const {
  uint32_t next_extension = header_size + 8 * 0;
  const uint32_t core_size = next_extension;

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}

uint64_t None::SerializeToBytes(glcr::ByteBuffer& bytes, uint64_t offset, glcr::CapBuffer& caps) const {
  uint32_t next_extension = header_size + 8 * 0;
  const uint32_t core_size = next_extension;
  uint64_t next_cap = 0;

  // The next extension pointer is the length of the message. 
  WriteHeader(bytes, offset, core_size, next_extension);

  return next_extension;
}