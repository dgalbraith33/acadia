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
  ~YellowstoneClient();

  z_cap_t Capability() { return endpoint_; }


  
  [[nodiscard]] glcr::ErrorCode RegisterEndpoint(const RegisterEndpointRequest& request);
  

  
  [[nodiscard]] glcr::ErrorCode GetEndpoint(const GetEndpointRequest& request, Endpoint& response);
  

  
  [[nodiscard]] glcr::ErrorCode GetAhciInfo(AhciInfo& response);
  

  
  [[nodiscard]] glcr::ErrorCode GetFramebufferInfo(FramebufferInfo& response);
  

  
  [[nodiscard]] glcr::ErrorCode GetDenali(DenaliInfo& response);
  

 private:
  z_cap_t endpoint_;
  uint64_t kBufferSize = 0x1000;
  glcr::ByteBuffer buffer_{kBufferSize};
  uint64_t kCapBufferSize = 0x10;
  glcr::CapBuffer cap_buffer_{kCapBufferSize};
};
