#include "mammoth/port.h"

#include <glacier/status/error.h>
#include <zcall.h>

#include "mammoth/debug.h"

glcr::ErrorOr<Port> Port::Create() {
  z_cap_t port;
  RET_ERR(ZPortCreate(&port));
  return Port(port);
}
Port::Port(uint64_t port_cap) : port_cap_(port_cap) {}

glcr::ErrorCode Port::WriteString(glcr::String str, uint64_t cap) {
  return ZPortSend(port_cap_, str.length() + 1, str.cstr(), 1, &cap);
}
