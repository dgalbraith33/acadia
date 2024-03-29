// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/status/status.h>
#include <glacier/container/vector.h>
#include <glacier/string/string.h>
#include <yunq/message_view.h>
#include <yunq/serialize.h>
#include <ztypes.h>


class KeyboardListener {
 public:
  KeyboardListener() {}
  // Delete copy and move until implemented.
  KeyboardListener(const KeyboardListener&) = delete;
  KeyboardListener(KeyboardListener&&) = default;
  KeyboardListener& operator=(KeyboardListener&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const z_cap_t& port_capability() const { return port_capability_; }
  z_cap_t& mutable_port_capability() { return port_capability_; }
  void set_port_capability(const z_cap_t& value) { port_capability_ = value; }

 private:
  z_cap_t port_capability_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};

