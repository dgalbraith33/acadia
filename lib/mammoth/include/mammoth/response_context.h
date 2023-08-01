#pragma once

#include <glacier/status/error.h>
#include <zcall.h>
#include <ztypes.h>

class ResponseContext {
 public:
  ResponseContext(z_cap_t reply_port) : reply_port_(reply_port) {}

  ResponseContext(ResponseContext&) = delete;

  template <typename T>
  glcr::ErrorCode WriteStruct(const T& response) {
    // FIXME: Here and below probably don't count as written on error.
    written_ = true;
    return ZReplyPortSend(reply_port_, sizeof(T), &response, 0, nullptr);
  }

  template <typename T>
  glcr::ErrorCode WriteStructWithCap(const T& response, z_cap_t capability) {
    written_ = true;
    return ZReplyPortSend(reply_port_, sizeof(T), &response, 1, &capability);
  }

  glcr::ErrorCode WriteError(glcr::ErrorCode code) {
    uint64_t response[2]{
        static_cast<uint64_t>(-1),
        code,
    };
    written_ = true;
    return ZReplyPortSend(reply_port_, sizeof(response), &response, 0, nullptr);
  }

  bool HasWritten() { return written_; }

 private:
  z_cap_t reply_port_;
  bool written_ = false;
};
