#pragma once

#include <stdint.h>

#include "include/ztypes.h"

struct ZChannelCreateResp {
  z_cap_t chan_cap1;
  z_cap_t chan_cap2;
};

struct ZMessage {
  uint64_t num_bytes;
  void* data;

  uint64_t num_caps;
  z_cap_t* caps;
};

struct ZChannelSendReq {
  z_cap_t chan_cap;
  ZMessage message;
};

struct ZChannelRecvReq {
  z_cap_t chan_cap;
  ZMessage message;
};

struct ZPortCreateResp {
  z_cap_t port_cap;
};

struct ZPortSendReq {
  z_cap_t port_cap;
  ZMessage message;
};

struct ZPortRecvReq {
  z_cap_t port_cap;
  ZMessage message;
};

struct ZIrqRegisterReq {
  uint64_t irq_num;
};

struct ZIrqRegisterResp {
  z_cap_t port_cap;
};

struct ZCapDuplicateReq {
  z_cap_t cap;
};

struct ZCapDuplicateResp {
  z_cap_t cap;
};
