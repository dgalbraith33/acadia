#include "mammoth/endpoint_server.h"

#include "mammoth/debug.h"

glcr::ErrorOr<glcr::UniquePtr<EndpointClient>> EndpointServer::CreateClient() {
  uint64_t client_cap;
  // FIXME: Restrict permissions to send-only here.
  RET_ERR(ZCapDuplicate(endpoint_cap_, &client_cap));
  return EndpointClient::AdoptEndpoint(client_cap);
}

glcr::ErrorCode EndpointServer::Receive(uint64_t* num_bytes, void* data,
                                        z_cap_t* reply_port_cap) {
  return ZEndpointRecv(endpoint_cap_, num_bytes, data, reply_port_cap);
}

glcr::ErrorCode EndpointServer::RunServer() {
  while (true) {
    uint64_t message_size = kBufferSize;
    uint64_t reply_port_cap = 0;
    RET_ERR(Receive(&message_size, recieve_buffer_, &reply_port_cap));

    RequestContext request(recieve_buffer_, message_size);
    ResponseContext response(reply_port_cap);
    // FIXME: Consider pumping these errors into the response as well.
    RET_ERR(HandleRequest(request, response));
    if (!response.HasWritten()) {
      dbgln("Returning without having written a response. Req type %x",
            request.request_id());
    }
  }
}
