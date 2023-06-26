#include "yellowstone_server.h"

#include <denali/denali.h>
#include <glacier/string/string.h>
#include <mammoth/debug.h>
#include <stdlib.h>

#include "hw/gpt.h"
#include "include/yellowstone.h"

namespace {

void ServerThreadBootstrap(void* yellowstone) {
  dbgln("Yellowstone server starting");
  static_cast<YellowstoneServer*>(yellowstone)->ServerThread();
}

void RegistrationThreadBootstrap(void* yellowstone) {
  dbgln("Yellowstone registration starting");
  static_cast<YellowstoneServer*>(yellowstone)->RegistrationThread();
}

glcr::ErrorCode HandleDenaliRegistration(z_cap_t endpoint_cap) {
  EndpointClient endpoint = EndpointClient::AdoptEndpoint(endpoint_cap);
  DenaliClient client(endpoint);
  GptReader reader(client);

  return reader.ParsePartitionTables();
}

}  // namespace

glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> YellowstoneServer::Create() {
  ASSIGN_OR_RETURN(auto server, EndpointServer::Create());
  ASSIGN_OR_RETURN(PortServer port, PortServer::Create());
  return glcr::UniquePtr<YellowstoneServer>(
      new YellowstoneServer(glcr::Move(server), port));
}

YellowstoneServer::YellowstoneServer(glcr::UniquePtr<EndpointServer> server,
                                     PortServer port)
    : server_(glcr::Move(server)), register_port_(port) {}

Thread YellowstoneServer::RunServer() {
  return Thread(ServerThreadBootstrap, this);
}

Thread YellowstoneServer::RunRegistration() {
  return Thread(RegistrationThreadBootstrap, this);
}

void YellowstoneServer::ServerThread() {
  while (true) {
    uint64_t num_bytes = kBufferSize;
    uint64_t reply_port_cap;
    // FIXME: Error handling.
    check(server_->Recieve(&num_bytes, server_buffer_, &reply_port_cap));
    YellowstoneGetReq* req =
        reinterpret_cast<YellowstoneGetReq*>(server_buffer_);
    switch (req->type) {
      case kYellowstoneGetAhci:
        dbgln("Yellowstone::GetAHCI");
        break;
      case kYellowstoneGetRegistration: {
        dbgln("Yellowstone::GetRegistration");
        auto client_or = register_port_.CreateClient();
        if (!client_or.ok()) {
          check(client_or.error());
        }
        YellowstoneGetRegistrationResp resp;
        uint64_t reg_cap = client_or.value().cap();
        check(ZReplyPortSend(reply_port_cap, sizeof(resp), &resp, 1, &reg_cap));
        break;
      }
      default:
        dbgln("Unknown request type: %x", req->type);
        break;
    }
  }
}

void YellowstoneServer::RegistrationThread() {
  while (true) {
    uint64_t num_bytes = kBufferSize;
    z_cap_t endpoint_cap;
    // FIXME: Better error handling here.
    check(register_port_.RecvCap(&num_bytes, registration_buffer_,
                                 &endpoint_cap));
    glcr::String name(registration_buffer_);
    if (name == "denali") {
      denali_cap_ = endpoint_cap;
      check(HandleDenaliRegistration(denali_cap_));
      continue;
    }

    if (name == "victoriafalls") {
      victoria_falls_cap_ = endpoint_cap;
      continue;
    }

    dbgln("[WARN] Got endpoint cap type:");
    dbgln(name.cstr());
  }
}

glcr::ErrorOr<EndpointClient> YellowstoneServer::GetServerClient() {
  return server_->CreateClient();
}
