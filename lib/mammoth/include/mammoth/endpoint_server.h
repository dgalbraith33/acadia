#pragma once

#include <glacier/status/error_or.h>
#include <ztypes.h>

#include "mammoth/endpoint_client.h"

class EndpointServer {
 public:
  static glcr::ErrorOr<EndpointServer> Create();
  static EndpointServer Adopt(z_cap_t endpoint_cap);

  glcr::ErrorOr<EndpointClient> CreateClient();

  // FIXME: Release Cap here.
  z_cap_t GetCap() { return endpoint_cap_; }

 private:
  z_cap_t endpoint_cap_;

  EndpointServer(z_cap_t cap) : endpoint_cap_(cap) {}
};
