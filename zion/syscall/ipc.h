#pragma once

#include <glacier/status/error.h>

#include "include/zcall.h"

glcr::ErrorCode ChannelCreate(ZChannelCreateReq* resp);
glcr::ErrorCode ChannelSend(ZChannelSendReq* req);
glcr::ErrorCode ChannelRecv(ZChannelRecvReq* req);

glcr::ErrorCode PortCreate(ZPortCreateReq* req);
glcr::ErrorCode PortSend(ZPortSendReq* req);
glcr::ErrorCode PortRecv(ZPortRecvReq* req);
glcr::ErrorCode PortPoll(ZPortPollReq* req);
glcr::ErrorCode IrqRegister(ZIrqRegisterReq* req);

glcr::ErrorCode EndpointCreate(ZEndpointCreateReq* req);
glcr::ErrorCode EndpointSend(ZEndpointSendReq* req);
glcr::ErrorCode EndpointRecv(ZEndpointRecvReq* req);

glcr::ErrorCode ReplyPortSend(ZReplyPortSendReq* req);
glcr::ErrorCode ReplyPortRecv(ZReplyPortRecvReq* req);
