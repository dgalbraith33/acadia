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

  template <typename T>
  z_err_t WriteMessage(const T& obj, uint64_t cap);

  glcr::ErrorCode WriteString(glcr::String str, uint64_t cap);

 private:
  uint64_t port_cap_;
};

template <typename T>
z_err_t Port::WriteMessage(const T& obj, uint64_t cap) {
  return ZPortSend(port_cap_, sizeof(obj), &obj, 1, &cap);
}
