// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/container/vector.h>
#include <glacier/string/string.h>
#include <ztypes.h>
class KeyboardListener {
 public:
  KeyboardListener() {}
  // Delete copy and move until implemented.
  KeyboardListener(const KeyboardListener&) = delete;
  KeyboardListener(KeyboardListener&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const z_cap_t& port_capability() const { return port_capability_; }
  void set_port_capability(const z_cap_t& value) { port_capability_ = value; }

 private:
  z_cap_t port_capability_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};