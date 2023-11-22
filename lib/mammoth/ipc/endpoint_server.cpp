#include "ipc/endpoint_server.h"

#include "mammoth/debug.h"

// Declared as friend in EndpointServer.
void EndpointServerThreadBootstrap(void* endpoint_server) {
  reinterpret_cast<EndpointServer*>(endpoint_server)->ServerThread();
}

glcr::ErrorOr<glcr::UniquePtr<EndpointClient>> EndpointServer::CreateClient() {
  uint64_t client_cap;
  RET_ERR(ZCapDuplicate(endpoint_cap_, ~(kZionPerm_Read), &client_cap));
  return EndpointClient::AdoptEndpoint(client_cap);
}

Thread EndpointServer::RunServer() {
  return Thread(EndpointServerThreadBootstrap, this);
}

void EndpointServer::ServerThread() {
  while (true) {
    uint64_t message_size = kBufferSize;
    uint64_t reply_port_cap = 0;
    uint64_t num_caps = 0;
    glcr::ErrorCode err = static_cast<glcr::ErrorCode>(
        ZEndpointRecv(endpoint_cap_, &message_size, recieve_buffer_, &num_caps,
                      nullptr, &reply_port_cap));
    if (err != glcr::OK) {
      dbgln("Error in receive: {x}", err);
      continue;
    }

    RequestContext request(recieve_buffer_, message_size);
    ResponseContext response(reply_port_cap);
    // FIXME: Consider pumping these errors into the response as well.
    check(HandleRequest(request, response));
    if (!response.HasWritten()) {
      dbgln("Returning without having written a response. Req type {x}",
            request.request_id());
    }
  }
}
