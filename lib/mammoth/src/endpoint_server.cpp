#include "mammoth/endpoint_server.h"

glcr::ErrorOr<EndpointServer> EndpointServer::Create() {
  uint64_t cap;
  RET_ERR(ZEndpointCreate(&cap));
  return EndpointServer(cap);
}

glcr::ErrorOr<EndpointClient> EndpointServer::CreateClient() {
  uint64_t client_cap;
  // FIXME: Restrict permissions to send-only here.
  RET_ERR(ZCapDuplicate(endpoint_cap_, &client_cap));
  return EndpointClient::AdoptEndpoint(client_cap);
}
