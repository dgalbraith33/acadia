#pragma once

#include <stdint.h>

#include "include/ztypes.h"

struct ZMessage {
  uint64_t num_bytes;
  void* data;

  uint64_t num_caps;
  z_cap_t* caps;
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
