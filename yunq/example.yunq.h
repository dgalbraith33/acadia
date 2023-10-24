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
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&);


  glcr::String path() { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

  uint64_t options() { return options_; }
  void set_options(const uint64_t& value) { options_ = value; }


 private:

  glcr::String path_;

  uint64_t options_;


}

class File {
 public:
  File() {}
  // Delete copy and move until implemented.
  File(const File&) = delete;
  File(File&&) = delete;

  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset); 
  void ParseFromBytes(const glcr::ByteBuffer&, uint64_t offset, const glcr::CapBuffer&);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset);
  uint64_t SerializeToBytes(glcr::ByteBuffer&, uint64_t offset, glcr::CapBuffer&);


  glcr::String path() { return path_; }
  void set_path(const glcr::String& value) { path_ = value; }

  uint64_t attrs() { return attrs_; }
  void set_attrs(const uint64_t& value) { attrs_ = value; }

  z_cap_t mem_cap() { return mem_cap_; }
  void set_mem_cap(const z_cap_t& value) { mem_cap_ = value; }


 private:

  glcr::String path_;

  uint64_t attrs_;

  z_cap_t mem_cap_;


}
