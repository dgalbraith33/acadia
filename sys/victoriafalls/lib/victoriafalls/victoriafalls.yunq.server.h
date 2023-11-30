// Generated File -- DO NOT MODIFY.
#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/proc/thread.h>
#include <ztypes.h>

#include "victoriafalls.yunq.h"
#include "victoriafalls.yunq.client.h"





class VFSServerBase {
 public:
  VFSServerBase(z_cap_t VFS_cap) : endpoint_(VFS_cap) {}
  VFSServerBase(const VFSServerBase&) = delete;
  VFSServerBase(VFSServerBase&&) = delete;
  virtual ~VFSServerBase();

  glcr::ErrorOr<z_cap_t> CreateClientCap();
  glcr::ErrorOr<VFSClient> CreateClient();

  [[nodiscard]] Thread RunServer();



  [[nodiscard]] virtual glcr::ErrorCode HandleOpenFile(const OpenFileRequest&, OpenFileResponse&) = 0;



  [[nodiscard]] virtual glcr::ErrorCode HandleGetDirectory(const GetDirectoryRequest&, Directory&) = 0;



 private:
  z_cap_t endpoint_;

  friend void VFSServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::ErrorCode HandleRequest(const glcr::ByteBuffer& request, const glcr::CapBuffer& req_caps,
                                              glcr::ByteBuffer& response, uint64_t& resp_length,
                                              glcr::CapBuffer& resp_caps);
};



