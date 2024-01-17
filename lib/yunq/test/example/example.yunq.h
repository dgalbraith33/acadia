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
class Types {
 public:
  Types() {}
  // Delete copy and move until implemented.
  Types(const Types&) = delete;
  Types(Types&&) = default;
  Types& operator=(Types&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const uint64_t& unsigned_int() const { return unsigned_int_; }
  uint64_t& mutable_unsigned_int() { return unsigned_int_; }
  void set_unsigned_int(const uint64_t& value) { unsigned_int_ = value; } 

  const int64_t& signed_int() const { return signed_int_; }
  int64_t& mutable_signed_int() { return signed_int_; }
  void set_signed_int(const int64_t& value) { signed_int_ = value; } 

  const glcr::String& str() const { return str_; }
  glcr::String& mutable_str() { return str_; }
  void set_str(const glcr::String& value) { str_ = value; }

 private:
  uint64_t unsigned_int_;
  int64_t signed_int_;
  glcr::String str_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class Cap {
 public:
  Cap() {}
  // Delete copy and move until implemented.
  Cap(const Cap&) = delete;
  Cap(Cap&&) = default;
  Cap& operator=(Cap&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const z_cap_t& cap() const { return cap_; }
  z_cap_t& mutable_cap() { return cap_; }
  void set_cap(const z_cap_t& value) { cap_ = value; }

 private:
  z_cap_t cap_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class Repeated {
 public:
  Repeated() {}
  // Delete copy and move until implemented.
  Repeated(const Repeated&) = delete;
  Repeated(Repeated&&) = default;
  Repeated& operator=(Repeated&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const;

  const glcr::Vector<uint64_t>& unsigned_ints() const { return unsigned_ints_; }
  glcr::Vector<uint64_t>& mutable_unsigned_ints() { return unsigned_ints_; }
  void add_unsigned_ints(const uint64_t& value) { unsigned_ints_.PushBack(value); }
  void add_unsigned_ints(uint64_t&& value) { unsigned_ints_.PushBack(glcr::Move(value)); }

 private:
  glcr::Vector<uint64_t> unsigned_ints_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};


}  // namepace ex
