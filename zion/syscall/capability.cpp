#include "syscall/capability.h"

#include "scheduler/scheduler.h"

z_err_t CapDuplicate(ZCapDuplicateReq* req) {
  auto& proc = gScheduler->CurrentProcess();
  auto cap = proc.GetCapability(req->cap_in);
  if (!cap) {
    return Z_ERR_CAP_NOT_FOUND;
  }
  *req->cap_out = proc.AddExistingCapability(cap);
  return Z_OK;
}
