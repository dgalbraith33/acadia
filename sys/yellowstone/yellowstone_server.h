#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/endpoint_server.h>
#include <mammoth/port.h>
#include <mammoth/thread.h>

class YellowstoneServer {
 public:
  static glcr::ErrorOr<YellowstoneServer> Create();

  Thread RunServer();
  Thread RunRegistration();

  void ServerThread();
  void RegistrationThread();

  glcr::ErrorOr<EndpointClient> GetServerClient();

 private:
  EndpointServer server_;
  Port register_port_;

  static const uint64_t kBufferSize = 128;
  uint8_t server_buffer_[kBufferSize];
  char registration_buffer_[kBufferSize];

  // TODO: Store these in a data structure.
  z_cap_t denali_cap_ = 0;
  z_cap_t victoria_falls_cap_ = 0;

  YellowstoneServer(EndpointServer server, Port port);
};
