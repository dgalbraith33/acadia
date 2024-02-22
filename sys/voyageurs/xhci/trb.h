#pragma once

#include "xhci/xhci.h"

XhciTrb CreateLinkTrb(uint64_t physical_address);

XhciTrb CreateEnableSlotTrb();
XhciTrb CreateNoOpCommandTrb();
