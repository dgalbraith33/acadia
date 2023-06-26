#pragma once

#include <glacier/container/pair.h>
#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <zcall.h>
#include <ztypes.h>

class EndpointClient {
 public:
  EndpointClient() = delete;
  EndpointClient(const EndpointClient&) = delete;
  EndpointClient& operator=(const EndpointClient&) = delete;

  static glcr::UniquePtr<EndpointClient> AdoptEndpoint(z_cap_t cap);

  template <typename Req, typename Resp>
  glcr::ErrorOr<glcr::Pair<Resp, z_cap_t>> CallEndpoint(const Req& req);

  z_cap_t GetCap() { return cap_; }

 private:
  EndpointClient(uint64_t cap) : cap_(cap) {}
  z_cap_t cap_;
};

template <typename Req, typename Resp>
glcr::ErrorOr<glcr::Pair<Resp, z_cap_t>> EndpointClient::CallEndpoint(
    const Req& req) {
  uint64_t reply_port_cap;
  RET_ERR(ZEndpointSend(cap_, sizeof(Req), &req, &reply_port_cap));

  Resp resp;
  z_cap_t cap = 0;
  uint64_t num_caps = 1;
  uint64_t num_bytes = sizeof(Resp);
  RET_ERR(ZReplyPortRecv(reply_port_cap, &num_bytes, &resp, &num_caps, &cap));

  if (num_bytes != sizeof(resp) || num_caps != 1) {
    return glcr::FAILED_PRECONDITION;
  }

  return glcr::Pair{resp, cap};
}
