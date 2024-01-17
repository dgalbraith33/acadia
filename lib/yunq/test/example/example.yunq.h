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


namespace ex {

class Basic {
 public:
  Basic() {}
  // Delete copy and move until implemented.
  Basic(const Basic&) = delete;
  Basic(Basic&&) = default;
  Basic& operator=(Basic&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const uint64_t& field() const { return field_; }
  uint64_t& mutable_field() { return field_; }
  void set_field(const uint64_t& value) { field_ = value; }

 private:
  uint64_t field_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};


}  // namepace ex
