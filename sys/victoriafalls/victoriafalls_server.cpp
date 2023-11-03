#include "victoriafalls_server.h"

#include <glacier/string/str_split.h>
#include <mammoth/debug.h>
#include <zcall.h>

glcr::ErrorOr<glcr::UniquePtr<VFSServer>> VFSServer::Create() {
  z_cap_t endpoint_cap;
  RET_ERR(ZEndpointCreate(&endpoint_cap));
  return glcr::UniquePtr<VFSServer>(new VFSServer(endpoint_cap));
}

glcr::ErrorCode VFSServer::HandleOpenFile(const OpenFileRequest& request,
                                          OpenFileResponse&) {
  auto path_tokens = glcr::StrSplit(request.path(), '/');
  for (uint64_t i = 0; i < path_tokens.size(); i++) {
    dbgln("Token %u: '%s'", i, glcr::String(path_tokens.at(i)).cstr());
  }
  // Require all paths to be absolute rather than relative.
  // If the path starts with '/' then the first token will be empty.
  if (path_tokens.at(0) != "") {
    return glcr::INVALID_ARGUMENT;
  }
  return glcr::UNIMPLEMENTED;
}
