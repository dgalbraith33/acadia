// Generated file - DO NOT MODIFY
#pragma once

#include <glacier/buffer/byte_buffer.h>
#include <glacier/buffer/cap_buffer.h>
#include <glacier/status/error.h>
#include <ztypes.h>

#include "voyageurs.yunq.h"

class VoyageursClient {
 public:
  VoyageursClient(z_cap_t Voyageurs_cap) : endpoint_(Voyageurs_cap) {}
  VoyageursClient(const VoyageursClient&) = delete;
  VoyageursClient(VoyageursClient&& other) : endpoint_(other.endpoint_) {other.endpoint_ = 0;};

  z_cap_t Capability() { return endpoint_; }


  [[nodiscard]] glcr::ErrorCode RegisterKeyboardListener(const KeyboardListener& request, None& response);

 private:
  z_cap_t endpoint_;
  uint64_t kBufferSize = 0x1000;
  glcr::ByteBuffer buffer_{kBufferSize};
  uint64_t kCapBufferSize = 0x10;
  glcr::CapBuffer cap_buffer_{kCapBufferSize};
};
