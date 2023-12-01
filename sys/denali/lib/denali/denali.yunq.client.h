// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/status/error.h>
#include <ztypes.h>

#include "denali.yunq.h"


class DenaliClient {
 public:
  DenaliClient(z_cap_t Denali_cap) : endpoint_(Denali_cap) {}
  DenaliClient(const DenaliClient&) = delete;
  DenaliClient(DenaliClient&& other) : endpoint_(other.endpoint_) {other.endpoint_ = 0;};
  ~DenaliClient();

  z_cap_t Capability() { return endpoint_; }


  
  [[nodiscard]] glcr::Status Read(const ReadRequest& request, ReadResponse& response);
  

  
  [[nodiscard]] glcr::Status ReadMany(const ReadManyRequest& request, ReadResponse& response);
  

 private:
  z_cap_t endpoint_;
  uint64_t kBufferSize = 0x1000;
  glcr::ByteBuffer buffer_{kBufferSize};
  uint64_t kCapBufferSize = 0x10;
  glcr::CapBuffer cap_buffer_{kCapBufferSize};
};

