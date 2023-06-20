#pragma once

#include "include/zcall.h"

z_err_t ChannelCreate(ZChannelCreateReq* resp);
z_err_t ChannelSend(ZChannelSendReq* req);
z_err_t ChannelRecv(ZChannelRecvReq* req);
