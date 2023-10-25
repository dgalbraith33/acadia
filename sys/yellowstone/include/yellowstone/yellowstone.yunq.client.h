// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/status/error.h>
#include <ztypes.h>

#include "yellowstone.yunq.h"

class YellowstoneClient {
 public:
  YellowstoneClient(z_cap_t Yellowstone_cap) : endpoint_(Yellowstone_cap) {}
  YellowstoneClient(const YellowstoneClient&) = delete;
  YellowstoneClient(YellowstoneClient&& other) : endpoint_(other.endpoint_) {other.endpoint_ = 0;};

  z_cap_t Capability() { return endpoint_; }


  [[nodiscard]] glcr::ErrorCode GetRegister(const Empty& request, RegisterInfo& response);

  [[nodiscard]] glcr::ErrorCode GetAhciInfo(const Empty& request, AhciInfo& response);

  [[nodiscard]] glcr::ErrorCode GetDenali(const Empty& request, DenaliInfo& response);

 private:
  z_cap_t endpoint_;
  uint64_t kBufferSize = 0x1000;
  glcr::ByteBuffer buffer_{kBufferSize};
  uint64_t kCapBufferSize = 0x10;
  glcr::CapBuffer cap_buffer_{kCapBufferSize};
};
