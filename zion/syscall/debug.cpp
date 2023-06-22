#include "syscall/debug.h"

#include <glacier/status/error.h>

#include "debug/debug.h"

z_err_t Debug(ZDebugReq* req) {
  dbgln("[Debug] %s", req->message);
  return glcr::OK;
}
