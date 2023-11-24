#include "syscall/debug.h"

#include <glacier/status/error.h>

#include "debug/debug.h"

z_err_t Debug(ZDebugReq* req) {
  dbgln_large("[Debug] {}", glcr::StringView(req->message, req->size));
  return glcr::OK;
}
