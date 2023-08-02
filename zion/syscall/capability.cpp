#include "syscall/capability.h"

#include <glacier/status/error.h>

#include "scheduler/scheduler.h"

z_err_t CapDuplicate(ZCapDuplicateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto cap = proc.GetCapability(req->cap_in);
  if (!cap) {
    return glcr::CAP_NOT_FOUND;
  }
  if (!(cap->permissions() & kZionPerm_Duplicate)) {
    return glcr::CAP_PERMISSION_DENIED;
  }
  *req->cap_out = proc.AddExistingCapability(cap);
  return glcr::OK;
}
