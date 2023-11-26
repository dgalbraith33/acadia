// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/container/vector.h>
#include <glacier/string/string.h>
#include <ztypes.h>
class OpenFileRequest {
 public:
  OpenFileRequest() {}
  // Delete copy and move until implemented.
  OpenFileRequest(const OpenFileRequest&) = delete;
  OpenFileRequest(OpenFileRequest&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const glcr::String& path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

 private:
  glcr::String path_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class OpenFileResponse {
 public:
  OpenFileResponse() {}
  // Delete copy and move until implemented.
  OpenFileResponse(const OpenFileResponse&) = delete;
  OpenFileResponse(OpenFileResponse&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const glcr::String& path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; } 
  const uint64_t& size() const { return size_; }
  void set_size(const uint64_t& value) { size_ = value; } 
  const z_cap_t& memory() const { return memory_; }
  void set_memory(const z_cap_t& value) { memory_ = value; }

 private:
  glcr::String path_;
  uint64_t size_;
  z_cap_t memory_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class GetDirectoryRequest {
 public:
  GetDirectoryRequest() {}
  // Delete copy and move until implemented.
  GetDirectoryRequest(const GetDirectoryRequest&) = delete;
  GetDirectoryRequest(GetDirectoryRequest&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const glcr::String& path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

 private:
  glcr::String path_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class Directory {
 public:
  Directory() {}
  // Delete copy and move until implemented.
  Directory(const Directory&) = delete;
  Directory(Directory&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  const glcr::String& filenames() const { return filenames_; }
  void set_filenames(const glcr::String& value) { filenames_ = value; }

 private:
  glcr::String filenames_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};