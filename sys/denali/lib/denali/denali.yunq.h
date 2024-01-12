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


class DiskBlock {
 public:
  DiskBlock() {}
  // Delete copy and move until implemented.
  DiskBlock(const DiskBlock&) = delete;
  DiskBlock(DiskBlock&&) = default;
  DiskBlock& operator=(DiskBlock&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const uint64_t& lba() const { return lba_; }
  uint64_t& mutable_lba() { return lba_; }
  void set_lba(const uint64_t& value) { lba_ = value; } 

  const uint64_t& size() const { return size_; }
  uint64_t& mutable_size() { return size_; }
  void set_size(const uint64_t& value) { size_ = value; }

 private:
  uint64_t lba_;
  uint64_t size_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class ReadRequest {
 public:
  ReadRequest() {}
  // Delete copy and move until implemented.
  ReadRequest(const ReadRequest&) = delete;
  ReadRequest(ReadRequest&&) = default;
  ReadRequest& operator=(ReadRequest&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const uint64_t& device_id() const { return device_id_; }
  uint64_t& mutable_device_id() { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; } 

  const DiskBlock& block() const { return block_; }
  DiskBlock& mutable_block() { return block_; }

 private:
  uint64_t device_id_;
  DiskBlock block_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class ReadManyRequest {
 public:
  ReadManyRequest() {}
  // Delete copy and move until implemented.
  ReadManyRequest(const ReadManyRequest&) = delete;
  ReadManyRequest(ReadManyRequest&&) = default;
  ReadManyRequest& operator=(ReadManyRequest&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const uint64_t& device_id() const { return device_id_; }
  uint64_t& mutable_device_id() { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; }

  const glcr::Vector<DiskBlock>& blocks() const { return blocks_; }
  glcr::Vector<DiskBlock>& mutable_blocks() { return blocks_; }
  void add_blocks(DiskBlock&& value) { blocks_.PushBack(glcr::Move(value)); }

 private:
  uint64_t device_id_;
  glcr::Vector<DiskBlock> blocks_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class ReadResponse {
 public:
  ReadResponse() {}
  // Delete copy and move until implemented.
  ReadResponse(const ReadResponse&) = delete;
  ReadResponse(ReadResponse&&) = default;
  ReadResponse& operator=(ReadResponse&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const uint64_t& device_id() const { return device_id_; }
  uint64_t& mutable_device_id() { return device_id_; }
  void set_device_id(const uint64_t& value) { device_id_ = value; } 

  const uint64_t& size() const { return size_; }
  uint64_t& mutable_size() { return size_; }
  void set_size(const uint64_t& value) { size_ = value; } 

  const z_cap_t& memory() const { return memory_; }
  z_cap_t& mutable_memory() { return memory_; }
  void set_memory(const z_cap_t& value) { memory_ = value; }

 private:
  uint64_t device_id_;
  uint64_t size_;
  z_cap_t memory_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};

