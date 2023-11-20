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

  *req->cap_out = proc.AddNewCapability(cap->raw_obj(),
                                        cap->permissions() & req->perm_mask);
  return glcr::OK;
}

z_err_t CapRelease(ZCapReleaseReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  if (proc.ReleaseCapability(req->cap).empty()) {
    return glcr::CAP_NOT_FOUND;
  }
  return glcr::OK;
}
