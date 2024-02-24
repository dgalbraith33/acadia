#pragma once

#include <glacier/status/error_or.h>
#include <ztypes.h>

#include "mammoth/ipc/port_client.h"

namespace mmth {

class PortServer {
 public:
  PortServer() {}
  static glcr::ErrorOr<PortServer> Create();
  static PortServer AdoptCap(z_cap_t cap);

  glcr::ErrorOr<PortClient> CreateClient();

  glcr::ErrorCode RecvCap(uint64_t* num_bytes, char* msg, uint64_t* cap);
  glcr::ErrorCode PollForIntCap(uint64_t* msg, uint64_t* cap);

  glcr::ErrorOr<char> RecvChar();
  glcr::ErrorOr<uint16_t> RecvUint16();

  z_cap_t cap() { return port_cap_; }

 private:
  z_cap_t port_cap_;

  PortServer(z_cap_t cap);
};

}  // namespace mmth
