#pragma once

#include <stdint.h>

struct ZProcessSpawnReq {
  uint64_t proc_cap;
};

struct ZProcessSpawnResp {
  uint64_t proc_cap;
  uint64_t as_cap;
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
  uint64_t as_cap;
  uint64_t offset;
  uint64_t mem_cap;
};

struct ZAddressSpaceMapResp {
  uint64_t vaddr;
};

struct ZMemoryObjectCreateReq {
  uint64_t size;
};

struct ZMemoryObjectCreateResp {
  uint64_t mem_cap;
};
