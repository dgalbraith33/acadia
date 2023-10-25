#include "syscall/synchronization.h"

#include "object/mutex.h"
#include "scheduler/scheduler.h"

glcr::ErrorCode MutexCreate(ZMutexCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  *req->mutex_cap = curr_proc.AddNewCapability(Mutex::Create());
  return glcr::OK;
}

glcr::ErrorCode MutexLock(ZMutexLockReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->mutex_cap);
  RET_ERR(ValidateCapability<Mutex>(cap, kZionPerm_Lock));

  auto mutex = cap->obj<Mutex>();
  mutex->Lock();
  return glcr::OK;
}

glcr::ErrorCode MutexRelease(ZMutexReleaseReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->mutex_cap);
  // TODO: We may not want a separate permission for releasing the mutex.
  RET_ERR(ValidateCapability<Mutex>(cap, kZionPerm_Release));

  auto mutex = cap->obj<Mutex>();
  mutex->Release();
  return glcr::OK;
}
