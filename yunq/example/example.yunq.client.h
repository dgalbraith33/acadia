// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/status/error.h>
#include <ztypes.h>

#include "example.yunq.h"


namespace srv::file {

class VFSClient {
 public:
  VFSClient(z_cap_t VFS_cap) : endpoint_(VFS_cap) {}
  VFSClient(const VFSClient&) = delete;
  VFSClient(VFSClient&& other) : endpoint_(other.endpoint_) {other.endpoint_ = 0;};
  ~VFSClient();

  z_cap_t Capability() { return endpoint_; }


  
  [[nodiscard]] glcr::ErrorCode open(const OpenFileRequest& request, File& response);
  

 private:
  z_cap_t endpoint_;
  uint64_t kBufferSize = 0x1000;
  glcr::ByteBuffer buffer_{kBufferSize};
  uint64_t kCapBufferSize = 0x10;
  glcr::CapBuffer cap_buffer_{kCapBufferSize};
};


}  // namepace srv::file
