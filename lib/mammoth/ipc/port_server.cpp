#include "ipc/port_server.h"

#include <zcall.h>

namespace mmth {

glcr::ErrorOr<PortServer> PortServer::Create() {
  z_cap_t port;
  RET_ERR(ZPortCreate(&port));
  return PortServer(port);
}

PortServer PortServer::AdoptCap(z_cap_t cap) { return PortServer(cap); }

PortServer::PortServer(z_cap_t port_cap) : port_cap_(port_cap) {}

glcr::ErrorOr<PortClient> PortServer::CreateClient() {
  z_cap_t new_port;
  RET_ERR(ZCapDuplicate(port_cap_, ~(kZionPerm_Read), &new_port));
  return PortClient::AdoptPort(new_port);
}

glcr::ErrorCode PortServer::RecvCap(uint64_t *num_bytes, char *msg,
                                    uint64_t *cap) {
  uint64_t caps = 1;
  RET_ERR(ZPortRecv(port_cap_, num_bytes, reinterpret_cast<uint8_t *>(msg),
                    &caps, cap));

  if (caps != 1) {
    return glcr::FAILED_PRECONDITION;
  }
  return glcr::OK;
}

glcr::ErrorCode PortServer::PollForIntCap(uint64_t *msg, uint64_t *cap) {
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

glcr::ErrorOr<char> PortServer::RecvChar() {
  uint64_t bytes = 1;
  uint64_t caps = 0;
  char byte;
  RET_ERR(ZPortRecv(port_cap_, &bytes, &byte, &caps, nullptr));
  return byte;
}

glcr::ErrorOr<uint16_t> PortServer::RecvUint16() {
  uint64_t bytes = 2;
  uint64_t caps = 0;
  uint16_t data;
  RET_ERR(ZPortRecv(port_cap_, &bytes, &data, &caps, nullptr));
  return data;
}

}  // namespace mmth
