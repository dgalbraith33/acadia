#include "include/zcall.h"

#include <stdint.h>

#include "usr/zcall_internal.h"

z_err_t SysCall2(uint64_t number, const void* first, const void* second) {
  z_err_t return_code;
  asm("syscall"
      : "=a"(return_code)
      : "D"(number), "S"(first), "d"(second)
      : "rcx", "r11");
  return return_code;
}

z_err_t SysCall0(uint64_t number) { return SysCall2(number, 0, 0); }

z_err_t SysCall1(uint64_t number, const void* first) {
  return SysCall2(number, first, 0);
}

z_err_t ZChannelCreate(z_cap_t* channel1, z_cap_t* channel2) {
  ZChannelCreateResp resp;
  z_err_t ret = SysCall2(Z_CHANNEL_CREATE, 0, &resp);
  *channel1 = resp.chan_cap1;
  *channel2 = resp.chan_cap2;
  return ret;
}

z_err_t ZChannelSend(z_cap_t chan_cap, uint64_t num_bytes, const void* data,
                     uint64_t num_caps, const z_cap_t* caps) {
  ZChannelSendReq req{
      .chan_cap = chan_cap,
      .message =
          {
              .num_bytes = num_bytes,
              .data = const_cast<void*>(data),
              .num_caps = num_caps,
              .caps = const_cast<z_cap_t*>(caps),
          },
  };
  return SysCall1(Z_CHANNEL_SEND, &req);
}

z_err_t ZChannelRecv(z_cap_t chan_cap, uint64_t num_bytes, void* data,
                     uint64_t num_caps, z_cap_t* caps, uint64_t* actual_bytes,
                     uint64_t* actual_caps) {
  ZChannelRecvReq req{
      .chan_cap = chan_cap,
      .message =
          {
              .num_bytes = num_bytes,
              .data = data,
              .num_caps = num_caps,
              .caps = caps,
          },
  };
  z_err_t ret = SysCall1(Z_CHANNEL_RECV, &req);
  *actual_bytes = req.message.num_bytes;
  *actual_caps = req.message.num_caps;
  return ret;
}

z_err_t ZPortCreate(z_cap_t* port_cap) {
  ZPortCreateResp resp;
  z_err_t ret = SysCall2(Z_PORT_CREATE, 0, &resp);
  *port_cap = resp.port_cap;
  return ret;
}

z_err_t ZPortSend(z_cap_t port_cap, uint64_t num_bytes, const void* data,
                  uint64_t num_caps, z_cap_t* caps) {
  ZPortSendReq req{.port_cap = port_cap,
                   .message = {
                       .num_bytes = num_bytes,
                       .data = const_cast<void*>(data),
                       .num_caps = num_caps,
                       .caps = caps,
                   }};
  return SysCall1(Z_PORT_SEND, &req);
}

z_err_t ZPortRecv(z_cap_t port_cap, uint64_t num_bytes, void* data,
                  uint64_t num_caps, z_cap_t* caps, uint64_t* actual_bytes,
                  uint64_t* actual_caps) {
  ZPortRecvReq req{
      .port_cap = port_cap,
      .message =
          {
              .num_bytes = num_bytes,
              .data = data,
              .num_caps = num_caps,
              .caps = caps,
          },
  };
  z_err_t ret = SysCall1(Z_PORT_RECV, &req);
  *actual_bytes = req.message.num_bytes;
  *actual_caps = req.message.num_caps;
  return ret;
}

z_err_t ZPortPoll(z_cap_t port_cap, uint64_t num_bytes, void* data,
                  uint64_t num_caps, z_cap_t* caps, uint64_t* actual_bytes,
                  uint64_t* actual_caps) {
  ZPortRecvReq req{
      .port_cap = port_cap,
      .message =
          {
              .num_bytes = num_bytes,
              .data = data,
              .num_caps = num_caps,
              .caps = caps,
          },
  };
  z_err_t ret = SysCall1(Z_PORT_POLL, &req);
  *actual_bytes = req.message.num_bytes;
  *actual_caps = req.message.num_caps;
  return ret;
}

z_err_t ZIrqRegister(uint64_t irq_num, z_cap_t* port_cap) {
  ZIrqRegisterReq req{
      .irq_num = irq_num,
  };
  ZIrqRegisterResp resp;
  z_err_t ret = SysCall2(Z_IRQ_REGISTER, &req, &resp);
  *port_cap = resp.port_cap;
  return ret;
}

z_err_t ZCapDuplicate(z_cap_t cap_in, z_cap_t* cap_out) {
  ZCapDuplicateReq req{
      .cap = cap_in,
  };
  ZCapDuplicateResp resp;
  z_err_t ret = SysCall2(Z_CAP_DUPLICATE, &req, &resp);
  *cap_out = resp.cap;
  return ret;
}

z_err_t ZDebug(const char* message) { return SysCall1(Z_DEBUG_PRINT, message); }
