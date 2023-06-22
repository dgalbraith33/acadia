#pragma once

#include <glacier/status/error_or.h>
#include <glacier/string/string.h>
#include <stdint.h>
#include <zcall.h>

// FIXME: Split send and receive.
class Port {
 public:
  static glcr::ErrorOr<Port> Create();
  Port(uint64_t port_cap);

  glcr::ErrorCode RecvCap(uint64_t* num_bytes, char* msg, uint64_t* cap);
  z_err_t PollForIntCap(uint64_t* msg, uint64_t* cap);

  template <typename T>
  z_err_t WriteMessage(const T& obj, uint64_t cap);

  glcr::ErrorCode WriteString(glcr::String str, uint64_t cap);

  // FIXME: We can't create error_ors of ints
  glcr::ErrorCode Duplicate(uint64_t* new_cap);

 private:
  uint64_t port_cap_;
};

template <typename T>
z_err_t Port::WriteMessage(const T& obj, uint64_t cap) {
  return ZPortSend(port_cap_, sizeof(obj), &obj, 1, &cap);
}
