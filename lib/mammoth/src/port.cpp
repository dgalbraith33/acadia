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

glcr::ErrorCode Port::RecvCap(uint64_t *num_bytes, char *msg, uint64_t *cap) {
  uint64_t caps = 1;
  RET_ERR(ZPortRecv(port_cap_, num_bytes, reinterpret_cast<uint8_t *>(msg),
                    &caps, cap));

  if (caps != 1) {
    return glcr::FAILED_PRECONDITION;
  }
  return glcr::OK;
}
z_err_t Port::PollForIntCap(uint64_t *msg, uint64_t *cap) {
  uint64_t bytes = sizeof(uint64_t);
  uint64_t caps = 1;
  RET_ERR(ZPortPoll(port_cap_, &bytes, reinterpret_cast<uint8_t *>(msg), &caps,
                    cap));

  if (bytes != sizeof(uint64_t)) {
    return glcr::FAILED_PRECONDITION;
  }
  if (caps != 1) {
    return glcr::FAILED_PRECONDITION;
  }
  return glcr::OK;
}
glcr::ErrorCode Port::WriteString(glcr::String str, uint64_t cap) {
  return ZPortSend(port_cap_, str.length() + 1, str.cstr(), 1, &cap);
}

glcr::ErrorCode Port::Duplicate(uint64_t *new_cap) {
  return ZCapDuplicate(port_cap_, new_cap);
}
