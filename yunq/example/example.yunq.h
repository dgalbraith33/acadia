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
  glcr::String path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }
  const glcr::Vector<uint64_t>& options() const { return options_; }
  void add_options(const uint64_t& value) { options_.PushBack(value); }

 private:
  glcr::String path_;
  glcr::Vector<uint64_t> options_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};
class File {
 public:
  File() {}
  // Delete copy and move until implemented.
  File(const File&) = delete;
  File(File&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset) const;
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&) const; 
  glcr::String path() const { return path_; }
  void set_path(const glcr::String& value) { path_ = value; } 
  uint64_t attrs() const { return attrs_; }
  void set_attrs(const uint64_t& value) { attrs_ = value; } 
  z_cap_t mem_cap() const { return mem_cap_; }
  void set_mem_cap(const z_cap_t& value) { mem_cap_ = value; }

 private:
  glcr::String path_;
  uint64_t attrs_;
  z_cap_t mem_cap_;

  // Parses everything except for caps.
  void ParseFromBytesInternal(const glcr::ByteBuffer&, uint64_t offset);
};