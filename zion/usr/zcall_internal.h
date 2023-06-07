#pragma once

#include <stdint.h>

struct ZProcessSpawnReq {
  uint64_t proc_cap;
  uint64_t bootstrap_cap;
};

struct ZProcessSpawnResp {
  uint64_t proc_cap;
  uint64_t vmas_cap;
  uint64_t bootstrap_cap;
};

struct ZThreadCreateReq {
  uint64_t proc_cap;
};

struct ZThreadCreateResp {
  uint64_t thread_cap;
};

struct ZThreadStartReq {
  uint64_t thread_cap;
  uint64_t entry;
  uint64_t arg1;
  uint64_t arg2;
};

struct ZAddressSpaceMapReq {
  uint64_t vmas_cap;
  uint64_t vmas_offset;
  uint64_t vmmo_cap;
};

struct ZAddressSpaceMapResp {
  uint64_t vaddr;
};

struct ZMemoryObjectCreateReq {
  uint64_t size;
};

struct ZMemoryObjectCreateResp {
  uint64_t vmmo_cap;
};

struct ZChannelCreateResp {
  uint64_t chan_cap1;
  uint64_t chan_cap2;
};

struct ZMessage {
  uint64_t type;

  uint64_t num_bytes;
  uint8_t* bytes;

  uint64_t num_caps;
  uint64_t* caps;
};

struct ZChannelSendReq {
  uint64_t chan_cap;
  ZMessage message;
};

struct ZChannelRecvReq {
  uint64_t chan_cap;
  ZMessage message;
};
