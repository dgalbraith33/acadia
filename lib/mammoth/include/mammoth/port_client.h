#pragma once

#include <glacier/status/error_or.h>
#include <glacier/string/string.h>
#include <stdint.h>
#include <zcall.h>

class PortClient {
 public:
  static PortClient AdoptPort(z_cap_t port_cap);

  template <typename T>
  z_err_t WriteMessage(const T& obj, z_cap_t cap);

  glcr::ErrorCode WriteString(glcr::String str, z_cap_t cap);

  z_cap_t cap() { return port_cap_; }

 private:
  z_cap_t port_cap_;

  PortClient(z_cap_t port_cap);
};

template <typename T>
z_err_t PortClient::WriteMessage(const T& obj, z_cap_t cap) {
  return ZPortSend(port_cap_, sizeof(obj), &obj, 1, &cap);
}
