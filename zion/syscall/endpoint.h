#pragma once

#include <glacier/status/error.h>

#include "include/zcall.h"

glcr::ErrorCode EndpointCreate(ZEndpointCreateReq* req);

glcr::ErrorCode EndpointSend(ZEndpointSendReq* req);

glcr::ErrorCode EndpointRecv(ZEndpointRecvReq* req);
