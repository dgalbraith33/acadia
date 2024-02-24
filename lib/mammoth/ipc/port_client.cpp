#include "ipc/port_client.h"

#include <glacier/status/error.h>
#include <zcall.h>

#include "util/debug.h"

namespace mmth {

PortClient PortClient::AdoptPort(z_cap_t cap) { return PortClient(cap); }
PortClient::PortClient(z_cap_t port_cap) : port_cap_(port_cap) {}

glcr::ErrorCode PortClient::WriteString(glcr::String str, z_cap_t cap) {
  return static_cast<glcr::ErrorCode>(
      ZPortSend(port_cap_, str.length() + 1, str.cstr(), 1, &cap));
}

glcr::ErrorCode PortClient::WriteByte(uint8_t byte) {
  return static_cast<glcr::ErrorCode>(
      ZPortSend(port_cap_, 1, &byte, 0, nullptr));
}

glcr::ErrorCode PortClient::Write(uint16_t data) {
  return static_cast<glcr::ErrorCode>(
      ZPortSend(port_cap_, 2, &data, 0, nullptr));
}

glcr::ErrorCode PortClient::Write(uint64_t data) {
  return static_cast<glcr::ErrorCode>(
      ZPortSend(port_cap_, 8, &data, 0, nullptr));
}
}  // namespace mmth
