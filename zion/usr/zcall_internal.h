#pragma once

#include <stdint.h>

#include "include/ztypes.h"

struct ZProcessSpawnReq {
  z_cap_t proc_cap;
  z_cap_t bootstrap_cap;
};

struct ZProcessSpawnResp {
  z_cap_t proc_cap;
  z_cap_t vmas_cap;
  z_cap_t bootstrap_cap;
};

struct ZThreadCreateReq {
  z_cap_t proc_cap;
};

struct ZThreadCreateResp {
  z_cap_t thread_cap;
};

struct ZThreadStartReq {
  z_cap_t thread_cap;
  uint64_t entry;
  uint64_t arg1;
  uint64_t arg2;
};

struct ZAddressSpaceMapReq {
  z_cap_t vmas_cap;
  z_cap_t vmas_offset;
  z_cap_t vmmo_cap;
};

struct ZAddressSpaceMapResp {
  uint64_t vaddr;
};

struct ZMemoryObjectCreateReq {
  uint64_t size;
};

struct ZMemoryObjectCreateResp {
  z_cap_t vmmo_cap;
};

struct ZMemoryObjectCreatePhysicalReq {
  uint64_t paddr;
  uint64_t size;
};

struct ZMemoryObjectCreatePhysicalResp {
  z_cap_t vmmo_cap;
  uint64_t paddr;
};

struct ZTempPcieConfigObjectCreateResp {
  z_cap_t vmmo_cap;
  uint64_t vmmo_size;
};

struct ZChannelCreateResp {
  z_cap_t chan_cap1;
  z_cap_t chan_cap2;
};

struct ZMessage {
  uint64_t type;

  uint64_t num_bytes;
  uint8_t* bytes;

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
