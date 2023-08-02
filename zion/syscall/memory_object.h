#pragma once

#include "include/zcall.h"

z_err_t MemoryObjectCreate(ZMemoryObjectCreateReq* req);
z_err_t MemoryObjectCreatePhysical(ZMemoryObjectCreatePhysicalReq* req);
z_err_t MemoryObjectCreateContiguous(ZMemoryObjectCreateContiguousReq* req);
