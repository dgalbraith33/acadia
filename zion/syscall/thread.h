#pragma once

#include <glacier/status/error.h>

#include "include/zcall.h"

glcr::ErrorCode ThreadCreate(ZThreadCreateReq* req);
glcr::ErrorCode ThreadStart(ZThreadStartReq* req);
glcr::ErrorCode ThreadExit(ZThreadExitReq*);
glcr::ErrorCode ThreadWait(ZThreadWaitReq* req);
