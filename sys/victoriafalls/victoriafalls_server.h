#pragma once

#include <glacier/memory/unique_ptr.h>

#include "victoriafalls/victoriafalls.yunq.server.h"

class VFSServer : public VFSServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<VFSServer>> Create();

  glcr::ErrorCode HandleOpenFile(const OpenFileRequest&,
                                 OpenFileResponse&) override;

 private:
  VFSServer(z_cap_t endpoint_cap) : VFSServerBase(endpoint_cap) {}
};
