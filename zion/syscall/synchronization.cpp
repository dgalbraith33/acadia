#include "syscall/synchronization.h"

#include "object/mutex.h"
#include "object/semaphore.h"
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
  RET_ERR(ValidateCapability<Mutex>(cap, kZionPerm_Release));

  auto mutex = cap->obj<Mutex>();
  mutex->Release();
  return glcr::OK;
}

glcr::ErrorCode SemaphoreCreate(ZSemaphoreCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  *req->semaphore_cap = curr_proc.AddNewCapability(Semaphore::Create());
  return glcr::OK;
}

glcr::ErrorCode SemaphoreWait(ZSemaphoreWaitReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->semaphore_cap);
  RET_ERR(ValidateCapability<Semaphore>(cap, kZionPerm_Wait));

  auto semaphore = cap->obj<Semaphore>();
  semaphore->Wait();
  return glcr::OK;
}

glcr::ErrorCode SemaphoreSignal(ZSemaphoreSignalReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->semaphore_cap);
  RET_ERR(ValidateCapability<Semaphore>(cap, kZionPerm_Signal));

  auto semaphore = cap->obj<Semaphore>();
  semaphore->Signal();
  return glcr::OK;
}
