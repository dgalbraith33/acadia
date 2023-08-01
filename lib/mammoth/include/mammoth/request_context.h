#pragma once

#include <glacier/status/error.h>
#include <stdint.h>

class RequestContext {
 public:
  RequestContext(void* buffer, uint64_t buffer_length)
      : buffer_(buffer), buffer_length_(buffer_length) {
    if (buffer_length_ < sizeof(uint64_t)) {
      request_id_ = -1;
    } else {
      request_id_ = *reinterpret_cast<uint64_t*>(buffer);
    }
  }

  uint64_t request_id() { return request_id_; }

  template <typename T>
  glcr::ErrorCode As(T** arg) {
    if (buffer_length_ < sizeof(T)) {
      return glcr::INVALID_ARGUMENT;
    }
    *arg = reinterpret_cast<T*>(buffer_);
    return glcr::OK;
  }

 private:
  uint64_t request_id_;
  void* buffer_;
  uint64_t buffer_length_;
};
