// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <glacier/status/status.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "yellowstone.yunq.h"
#include "yellowstone.yunq.client.h"


namespace yellowstone {




class YellowstoneServerBase {
 public:
  YellowstoneServerBase(z_cap_t Yellowstone_cap) : endpoint_(Yellowstone_cap) {}
  YellowstoneServerBase(const YellowstoneServerBase&) = delete;
  YellowstoneServerBase(YellowstoneServerBase&&) = delete;
  virtual ~YellowstoneServerBase();

  glcr::ErrorOr<z_cap_t> CreateClientCap();
  glcr::ErrorOr<YellowstoneClient> CreateClient();

  [[nodiscard]] Thread RunServer();



  [[nodiscard]] virtual glcr::Status HandleRegisterEndpoint(const RegisterEndpointRequest&) = 0;



  [[nodiscard]] virtual glcr::Status HandleGetEndpoint(const GetEndpointRequest&, Endpoint&) = 0;



  [[nodiscard]] virtual glcr::Status HandleGetAhciInfo(AhciInfo&) = 0;



  [[nodiscard]] virtual glcr::Status HandleGetFramebufferInfo(FramebufferInfo&) = 0;



  [[nodiscard]] virtual glcr::Status HandleGetDenali(DenaliInfo&) = 0;



 private:
  z_cap_t endpoint_;

  friend void YellowstoneServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::Status HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                           glcr::ByteBuffer& response, uint64_t& resp_length,
                                           glcr::CapBuffer& resp_caps);
};




}  // namepace yellowstone
