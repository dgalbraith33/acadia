#include "ipc/port_client.h"

#include <glacier/status/error.h>
#include <zcall.h>

#include "util/debug.h"

PortClient PortClient::AdoptPort(z_cap_t cap) { return PortClient(cap); }
PortClient::PortClient(z_cap_t port_cap) : port_cap_(port_cap) {}

glcr::ErrorCode PortClient::WriteString(glcr::String str, z_cap_t cap) {
  return static_cast<glcr::ErrorCode>(
      ZPortSend(port_cap_, str.length() + 1, str.cstr(), 1, &cap));
}
