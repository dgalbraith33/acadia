#include "xhci/trb.h"

constexpr uint8_t kTrb_Normal = 1;
constexpr uint8_t kTrb_SetupStage = 2;
constexpr uint8_t kTrb_DataStage = 3;
constexpr uint8_t kTrb_StatusStage = 4;
constexpr uint8_t kTrb_Isoch = 5;
constexpr uint8_t kTrb_Link = 6;
constexpr uint8_t kTrb_EventData = 7;
constexpr uint8_t kTrb_NoOp = 8;
constexpr uint8_t kTrb_EnableSlot = 9;
constexpr uint8_t kTrb_DisableSlot = 10;
constexpr uint8_t kTrb_NoOpCommand = 23;

constexpr uint8_t kTrb_TypeOffset = 10;

constexpr uint8_t kTrb_Cycle = 1;

XhciTrb CreateLinkTrb(uint64_t physical_address) {
  return {
      .parameter = physical_address,
      .status = 0,
      .type_and_cycle = kTrb_Link << kTrb_TypeOffset,
      .control = 0,
  };
}

XhciTrb CreateEnableSlotTrb() {
  return {
      .parameter = 0,
      .status = 0,
      // FIXME: Accept Cycle Bit as a parameter.
      .type_and_cycle = kTrb_EnableSlot << kTrb_TypeOffset | kTrb_Cycle,
      // FIXME: Specify slot type if necessary. (XHCI Table 7-9)?
      .control = 0,
  };
}

XhciTrb CreateNoOpCommandTrb() {
  return {
      .parameter = 0,
      .status = 0,
      // FIXME: Accept Cycle Bit as a parameter.
      .type_and_cycle = kTrb_NoOpCommand << kTrb_TypeOffset | kTrb_Cycle,
      .control = 0,
  };
}
