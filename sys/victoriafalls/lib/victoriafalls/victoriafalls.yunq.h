// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
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
  glcr::String path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

 private:
  glcr::String path_;

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
  glcr::String path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }
  z_cap_t memory() const { return memory_; }
  void set_memory(const z_cap_t& value) { memory_ = value; }

 private:
  glcr::String path_;
  z_cap_t memory_;

};