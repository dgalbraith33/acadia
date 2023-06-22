#include "mammoth/endpoint_server.h"

EndpointClient EndpointClient::AdoptEndpoint(z_cap_t cap) { return {cap}; }
