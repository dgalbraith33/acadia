#pragma once

#include "include/zcall.h"

z_err_t PortCreate(ZPortCreateReq* req);
z_err_t PortSend(ZPortSendReq* req);
z_err_t PortRecv(ZPortRecvReq* req);
z_err_t PortPoll(ZPortPollReq* req);
z_err_t IrqRegister(ZIrqRegisterReq* req);
