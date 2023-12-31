#pragma once

#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <ztypes.h>

#include "mammoth/ipc/endpoint_client.h"
#include "mammoth/ipc/request_context.h"
#include "mammoth/ipc/response_context.h"
#include "mammoth/proc/thread.h"

namespace mmth {
class EndpointServer {
 public:
  EndpointServer() = delete;
  EndpointServer(const EndpointServer&) = delete;
  EndpointServer& operator=(const EndpointServer&) = delete;

  glcr::ErrorOr<glcr::UniquePtr<EndpointClient>> CreateClient();

  Thread RunServer();

  virtual glcr::ErrorCode HandleRequest(RequestContext& request,
                                        ResponseContext& response) = 0;

 protected:
  EndpointServer(z_cap_t cap) : endpoint_cap_(cap) {}

 private:
  z_cap_t endpoint_cap_;

  static const uint64_t kBufferSize = 1024;
  uint8_t recieve_buffer_[kBufferSize];

  friend void EndpointServerThreadBootstrap(void* endpoint_server);
  void ServerThread();
};

}  // namespace mmth
