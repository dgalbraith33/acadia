// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "yellowstone.yunq.h"
#include "yellowstone.yunq.client.h"



class YellowstoneServerBase {
 public:
  YellowstoneServerBase(z_cap_t Yellowstone_cap) : endpoint_(Yellowstone_cap) {}
  YellowstoneServerBase(const YellowstoneServerBase&) = delete;
  YellowstoneServerBase(YellowstoneServerBase&&) = delete;
  virtual ~YellowstoneServerBase();

  glcr::ErrorOr<YellowstoneClient> CreateClient();

  [[nodiscard]] Thread RunServer();



  [[nodiscard]] virtual glcr::ErrorCode HandleRegisterEndpoint(const RegisterEndpointRequest&) = 0;



  [[nodiscard]] virtual glcr::ErrorCode HandleGetEndpoint(const GetEndpointRequest&, Endpoint&) = 0;



  [[nodiscard]] virtual glcr::ErrorCode HandleGetAhciInfo(AhciInfo&) = 0;



  [[nodiscard]] virtual glcr::ErrorCode HandleGetFramebufferInfo(FramebufferInfo&) = 0;



  [[nodiscard]] virtual glcr::ErrorCode HandleGetDenali(DenaliInfo&) = 0;



 private:
  z_cap_t endpoint_;

  friend void YellowstoneServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::ErrorCode HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                              glcr::ByteBuffer& response, uint64_t& resp_length,
                                              glcr::CapBuffer& resp_caps);
};

