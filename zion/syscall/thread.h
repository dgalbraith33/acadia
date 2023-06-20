#pragma once

#include "include/zcall.h"

z_err_t ThreadCreate(ZThreadCreateReq* req);
z_err_t ThreadStart(ZThreadStartReq* req);
z_err_t ThreadExit(ZThreadExitReq*);
