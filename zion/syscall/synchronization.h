#pragma once

#include <glacier/status/error.h>

#include "include/zcall.h"

glcr::ErrorCode MutexCreate(ZMutexCreateReq* req);
glcr::ErrorCode MutexLock(ZMutexLockReq* req);
glcr::ErrorCode MutexRelease(ZMutexReleaseReq* req);

glcr::ErrorCode SemaphoreCreate(ZSemaphoreCreateReq* req);
glcr::ErrorCode SemaphoreWait(ZSemaphoreWaitReq* req);
glcr::ErrorCode SemaphoreSignal(ZSemaphoreSignalReq* req);
