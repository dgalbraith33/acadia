#include "mammoth/endpoint_server.h"

glcr::UniquePtr<EndpointClient> EndpointClient::AdoptEndpoint(z_cap_t cap) {
  return glcr::UniquePtr<EndpointClient>(new EndpointClient(cap));
}
