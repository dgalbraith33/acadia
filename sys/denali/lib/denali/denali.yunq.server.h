// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "denali.yunq.h"
#include "denali.yunq.client.h"



class DenaliServerBase {
 public:
  DenaliServerBase(z_cap_t Denali_cap) : endpoint_(Denali_cap) {}
  DenaliServerBase(const DenaliServerBase&) = delete;
  DenaliServerBase(DenaliServerBase&&) = delete;

  glcr::ErrorOr<DenaliClient> CreateClient();

  [[nodiscard]] Thread RunServer();


  [[nodiscard]] virtual glcr::ErrorCode HandleRead(const ReadRequest&, ReadResponse&) = 0;

  [[nodiscard]] virtual glcr::ErrorCode HandleReadMany(const ReadManyRequest&, ReadResponse&) = 0;


 private:
  z_cap_t endpoint_;

  friend void DenaliServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::ErrorCode HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                              glcr::ByteBuffer& response, uint64_t& resp_length,
                                              glcr::CapBuffer& resp_caps);
};

