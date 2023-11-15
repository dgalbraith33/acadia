// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/container/vector.h>
#include <glacier/string/string.h>
#include <ztypes.h>
class ReadRequest {
 public:
  ReadRequest() {}
  // Delete copy and move until implemented.
  ReadRequest(const ReadRequest&) = delete;
  ReadRequest(ReadRequest&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  uint64_t device_id() const { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; } 
  uint64_t lba() const { return lba_; }
  void set_lba(const uint64_t& value) { lba_ = value; } 
  uint64_t size() const { return size_; }
  void set_size(const uint64_t& value) { size_ = value; }

 private:
  uint64_t device_id_;
  uint64_t lba_;
  uint64_t size_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class ReadManyRequest {
 public:
  ReadManyRequest() {}
  // Delete copy and move until implemented.
  ReadManyRequest(const ReadManyRequest&) = delete;
  ReadManyRequest(ReadManyRequest&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  uint64_t device_id() const { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; }
  const glcr::Vector<uint64_t>& lba() const { return lba_; }
  void add_lba(const uint64_t& value) { lba_.PushBack(value); }

 private:
  uint64_t device_id_;
  glcr::Vector<uint64_t> lba_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class ReadResponse {
 public:
  ReadResponse() {}
  // Delete copy and move until implemented.
  ReadResponse(const ReadResponse&) = delete;
  ReadResponse(ReadResponse&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  uint64_t device_id() const { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; } 
  uint64_t size() const { return size_; }
  void set_size(const uint64_t& value) { size_ = value; } 
  z_cap_t memory() const { return memory_; }
  void set_memory(const z_cap_t& value) { memory_ = value; }

 private:
  uint64_t device_id_;
  uint64_t size_;
  z_cap_t memory_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};