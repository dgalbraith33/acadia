// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <glacier/status/status.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "voyageurs.yunq.h"
#include "voyageurs.yunq.client.h"





class VoyageursServerBase {
 public:
  VoyageursServerBase(z_cap_t Voyageurs_cap) : endpoint_(Voyageurs_cap) {}
  VoyageursServerBase(const VoyageursServerBase&) = delete;
  VoyageursServerBase(VoyageursServerBase&&) = delete;
  virtual ~VoyageursServerBase();

  glcr::ErrorOr<z_cap_t> CreateClientCap();
  glcr::ErrorOr<VoyageursClient> CreateClient();

  [[nodiscard]] Thread RunServer();



  [[nodiscard]] virtual glcr::Status HandleRegisterKeyboardListener(const KeyboardListener&) = 0;



 private:
  z_cap_t endpoint_;

  friend void VoyageursServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::Status HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                           glcr::ByteBuffer& response, uint64_t& resp_length,
                                           glcr::CapBuffer& resp_caps);
};



