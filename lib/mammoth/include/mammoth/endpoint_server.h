#pragma once

#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <ztypes.h>

#include "mammoth/endpoint_client.h"

class EndpointServer {
 public:
  EndpointServer() = delete;
  EndpointServer(const EndpointServer&) = delete;
  EndpointServer& operator=(const EndpointServer&) = delete;

  static glcr::ErrorOr<glcr::UniquePtr<EndpointServer>> Create();
  static glcr::UniquePtr<EndpointServer> Adopt(z_cap_t endpoint_cap);

  glcr::ErrorOr<EndpointClient> CreateClient();

  // FIXME: Release Cap here.
  z_cap_t GetCap() { return endpoint_cap_; }

  glcr::ErrorCode Recieve(uint64_t* num_bytes, void* data,
                          z_cap_t* reply_port_cap);

 private:
  z_cap_t endpoint_cap_;

  EndpointServer(z_cap_t cap) : endpoint_cap_(cap) {}
};
