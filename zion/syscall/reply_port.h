#pragma once

#include <glacier/status/error.h>

#include "include/zcall.h"

glcr::ErrorCode ReplyPortSend(ZReplyPortSendReq* req);

glcr::ErrorCode ReplyPortRecv(ZReplyPortRecvReq* req);
