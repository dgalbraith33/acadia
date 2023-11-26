#pragma once

#include <glacier/memory/unique_ptr.h>

#include "fs/ext2/ext2_driver.h"
#include "victoriafalls/victoriafalls.yunq.server.h"

class VFSServer : public VFSServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<VFSServer>> Create(Ext2Driver& driver);

  glcr::ErrorCode HandleOpenFile(const OpenFileRequest&,
                                 OpenFileResponse&) override;

  glcr::ErrorCode HandleGetDirectory(const GetDirectoryRequest&,
                                     Directory&) override;

 private:
  // FIXME: Don't store this as a reference.
  Ext2Driver& driver_;

  VFSServer(z_cap_t endpoint_cap, Ext2Driver& driver)
      : VFSServerBase(endpoint_cap), driver_(driver) {}
};
