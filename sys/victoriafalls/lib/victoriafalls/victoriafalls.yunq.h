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


class OpenFileRequest {
 public:
  OpenFileRequest() {}
  // Delete copy and move until implemented.
  OpenFileRequest(const OpenFileRequest&) = delete;
  OpenFileRequest(OpenFileRequest&&) = default;
  OpenFileRequest& operator=(OpenFileRequest&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const glcr::String& path() const { return path_; }
  glcr::String& mutable_path() { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

 private:
  glcr::String path_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class OpenFileResponse {
 public:
  OpenFileResponse() {}
  // Delete copy and move until implemented.
  OpenFileResponse(const OpenFileResponse&) = delete;
  OpenFileResponse(OpenFileResponse&&) = default;
  OpenFileResponse& operator=(OpenFileResponse&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const glcr::String& path() const { return path_; }
  glcr::String& mutable_path() { return path_; }
  void set_path(const glcr::String& value) { path_ = value; } 

  const uint64_t& size() const { return size_; }
  uint64_t& mutable_size() { return size_; }
  void set_size(const uint64_t& value) { size_ = value; } 

  const z_cap_t& memory() const { return memory_; }
  z_cap_t& mutable_memory() { return memory_; }
  void set_memory(const z_cap_t& value) { memory_ = value; }

 private:
  glcr::String path_;
  uint64_t size_;
  z_cap_t memory_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class GetDirectoryRequest {
 public:
  GetDirectoryRequest() {}
  // Delete copy and move until implemented.
  GetDirectoryRequest(const GetDirectoryRequest&) = delete;
  GetDirectoryRequest(GetDirectoryRequest&&) = default;
  GetDirectoryRequest& operator=(GetDirectoryRequest&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const glcr::String& path() const { return path_; }
  glcr::String& mutable_path() { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

 private:
  glcr::String path_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};
class Directory {
 public:
  Directory() {}
  // Delete copy and move until implemented.
  Directory(const Directory&) = delete;
  Directory(Directory&&) = default;
  Directory& operator=(Directory&&) = default;

  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message);
  [[nodiscard]] glcr::Status ParseFromBytes(const yunq::MessageView& message, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 

  const glcr::String& filenames() const { return filenames_; }
  glcr::String& mutable_filenames() { return filenames_; }
  void set_filenames(const glcr::String& value) { filenames_ = value; }

 private:
  glcr::String filenames_;

  // Parses everything except for caps.
  glcr::Status ParseFromBytesInternal(const yunq::MessageView& message);

  uint64_t SerializeInternal(yunq::Serializer& serializer) const;
};

