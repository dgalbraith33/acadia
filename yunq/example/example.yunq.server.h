// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <glacier/status/status.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "example.yunq.h"
#include "example.yunq.client.h"


namespace srv::file {




class VFSServerBase {
 public:
  VFSServerBase(z_cap_t VFS_cap) : endpoint_(VFS_cap) {}
  VFSServerBase(const VFSServerBase&) = delete;
  VFSServerBase(VFSServerBase&&) = delete;
  virtual ~VFSServerBase();

  glcr::ErrorOr<z_cap_t> CreateClientCap();
  glcr::ErrorOr<VFSClient> CreateClient();

  [[nodiscard]] Thread RunServer();



  [[nodiscard]] virtual glcr::Status Handleopen(const OpenFileRequest&, File&) = 0;



 private:
  z_cap_t endpoint_;

  friend void VFSServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::Status HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                           glcr::ByteBuffer& response, uint64_t& resp_length,
                                           glcr::CapBuffer& resp_caps);
};




}  // namepace srv::file
