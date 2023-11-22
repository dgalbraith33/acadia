#include "ipc/endpoint_server.h"

namespace mmth {

glcr::UniquePtr<EndpointClient> EndpointClient::AdoptEndpoint(z_cap_t cap) {
  return glcr::UniquePtr<EndpointClient>(new EndpointClient(cap));
}

}  // namespace mmth
