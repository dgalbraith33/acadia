#include "xhci/trb.h"

XhciTrb CreateLinkTrb(uint64_t physical_address) {
  return {
      .parameter = physical_address,
      .status = 0,
      .type_and_cycle = 6 << 10,
      .control = 0,
  };
}
