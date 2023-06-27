#pragma once

#include <stdint.h>

const uint64_t kYellowstoneGetRegistration = 0x01;
const uint64_t kYellowstoneGetAhci = 0x02;

struct YellowstoneGetReq {
  uint64_t type;
};

struct YellowstoneGetRegistrationResp {};
struct YellowstoneGetAhciResp {
  uint64_t type;
  uint64_t ahci_phys_offset;
};
