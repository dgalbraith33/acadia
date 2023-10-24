// Generated file - DO NOT MODIFY
#pragma once

#include <ztypes.h>

#include "example.yunq.h"

class VFSClient {
 public:
  VFSClient(z_cap_t VFS_cap) : endpoint_(VFS_cap) {}
  VFSClient(const VFSClient&) = delete;
  VFSClient(VFSClient&&) = delete;

  [[nodiscard]] glcr::ErrorCode open(const OpenFileRequest& request, File& response);

 private:
  z_cap_t endpoint_;
};
