// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <glacier/status/status.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "denali.yunq.h"
#include "denali.yunq.client.h"





class DenaliServerBase {
 public:
  DenaliServerBase(z_cap_t Denali_cap) : endpoint_(Denali_cap) {}
  DenaliServerBase(const DenaliServerBase&) = delete;
  DenaliServerBase(DenaliServerBase&&) = delete;
  virtual ~DenaliServerBase();

  glcr::ErrorOr<z_cap_t> CreateClientCap();
  glcr::ErrorOr<DenaliClient> CreateClient();

  [[nodiscard]] Thread RunServer();



  [[nodiscard]] virtual glcr::Status HandleRead(const ReadRequest&, ReadResponse&) = 0;



  [[nodiscard]] virtual glcr::Status HandleReadMany(const ReadManyRequest&, ReadResponse&) = 0;



 private:
  z_cap_t endpoint_;

  friend void DenaliServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::Status HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                           glcr::ByteBuffer& response, uint64_t& resp_length,
                                           glcr::CapBuffer& resp_caps);
};



