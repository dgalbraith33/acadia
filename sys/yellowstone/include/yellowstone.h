#pragma once

#include <stdint.h>

const uint64_t kYellowstoneGetRegistration = 0x01;
const uint64_t kYellowstoneGetAhci = 0x02;
const uint64_t kYellowstoneGetDenali = 0x03;

struct YellowstoneGetReq {
  uint64_t type;
};

struct YellowstoneGetRegistrationResp {};
struct YellowstoneGetAhciResp {
  uint64_t type;
  uint64_t ahci_length;
};

// Has a denali cap attached.
struct YellowstoneGetDenaliResp {
  uint64_t type;
  uint64_t device_id;
  uint64_t lba_offset;
};
