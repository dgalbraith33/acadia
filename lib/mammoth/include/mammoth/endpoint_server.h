#pragma once

#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <ztypes.h>

#include "mammoth/endpoint_client.h"
#include "mammoth/request_context.h"
#include "mammoth/response_context.h"

class EndpointServer {
 public:
  EndpointServer() = delete;
  EndpointServer(const EndpointServer&) = delete;
  EndpointServer& operator=(const EndpointServer&) = delete;

  glcr::ErrorOr<glcr::UniquePtr<EndpointClient>> CreateClient();
  // FIXME: Release Cap here.
  z_cap_t GetCap() { return endpoint_cap_; }

  glcr::ErrorCode Receive(uint64_t* num_bytes, void* data,
                          z_cap_t* reply_port_cap);

  glcr::ErrorCode RunServer();

  virtual glcr::ErrorCode HandleRequest(RequestContext& request,
                                        ResponseContext& response) = 0;

 protected:
  EndpointServer(z_cap_t cap) : endpoint_cap_(cap) {}

 private:
  z_cap_t endpoint_cap_;

  static const uint64_t kBufferSize = 1024;
  uint8_t recieve_buffer_[kBufferSize];
};
