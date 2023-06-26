#include "mammoth/endpoint_server.h"

glcr::ErrorOr<glcr::UniquePtr<EndpointServer>> EndpointServer::Create() {
  uint64_t cap;
  RET_ERR(ZEndpointCreate(&cap));
  return glcr::UniquePtr<EndpointServer>(new EndpointServer(cap));
}

glcr::UniquePtr<EndpointServer> EndpointServer::Adopt(z_cap_t endpoint_cap) {
  return glcr::UniquePtr<EndpointServer>(new EndpointServer(endpoint_cap));
}

glcr::ErrorOr<glcr::UniquePtr<EndpointClient>> EndpointServer::CreateClient() {
  uint64_t client_cap;
  // FIXME: Restrict permissions to send-only here.
  RET_ERR(ZCapDuplicate(endpoint_cap_, &client_cap));
  return EndpointClient::AdoptEndpoint(client_cap);
}

glcr::ErrorCode EndpointServer::Recieve(uint64_t* num_bytes, void* data,
                                        z_cap_t* reply_port_cap) {
  return ZEndpointRecv(endpoint_cap_, num_bytes, data, reply_port_cap);
}
