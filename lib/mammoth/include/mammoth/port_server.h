#pragma once

#include <glacier/status/error_or.h>
#include <ztypes.h>

class PortServer {
 public:
  static glcr::ErrorOr<PortServer> Create();
  static PortServer AdoptCap(z_cap_t cap);

  glcr::ErrorCode CreateClient(z_cap_t* new_port);

  glcr::ErrorCode RecvCap(uint64_t* num_bytes, char* msg, uint64_t* cap);
  glcr::ErrorCode PollForIntCap(uint64_t* msg, uint64_t* cap);

 private:
  z_cap_t port_cap_;

  PortServer(z_cap_t cap);
};
