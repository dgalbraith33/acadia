#include "victoriafalls_server.h"

#include <zcall.h>

glcr::ErrorOr<glcr::UniquePtr<VFSServer>> VFSServer::Create() {
  z_cap_t endpoint_cap;
  RET_ERR(ZEndpointCreate(&endpoint_cap));
  return glcr::UniquePtr<VFSServer>(new VFSServer(endpoint_cap));
}

glcr::ErrorCode VFSServer::HandleOpenFile(const OpenFileRequest&,
                                          OpenFileResponse&) {
  return glcr::OK;
}
