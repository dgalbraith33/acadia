// Generated File -- DO NOT MODIFY.
#pragma once

#include <mammoth/thread.h>
#include <ztypes.h>

#include "example.yunq.h"



class VFSServerBase {
 public:
  VFSServerBase(z_cap_t VFS_cap) : {}
  VFSServerBase(const VFSServerBase&) = delete;
  VFSServerBase(VFSServerBase&&) = delete;

  [[nodiscard]] Thread RunServer();


  [[nodiscard]] virtual glcr::ErrorCode Handleopen(const OpenFileRequest&, File&) = 0;


 private:
  z_cap_t endpoint_;

  friend void VFSServerBaseThreadBootstrap(void*);
  void ServerThread();

  [[nodiscard]] glcr::ErrorCode HandleRequest(const glcr::ByteBuffer& request, glcr::ByteBuffer& response,
                                              glcr::CapBuffer& resp_caps);
};

