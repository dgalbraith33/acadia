#include "xhci/trb.h"

constexpr uint8_t kTrb_TypeOffset = 10;

constexpr uint16_t kTrb_Cycle = 1;
constexpr uint16_t kTrb_ToggleCycle = (1 << 1);
constexpr uint16_t kTrb_BSR = (1 << 9);

namespace {

uint16_t TypeToInt(TrbType type) {
  return static_cast<uint16_t>(type) << kTrb_TypeOffset;
}

}  // namespace

TrbType GetType(const XhciTrb& trb) {
  return TrbType(trb.type_and_cycle >> kTrb_TypeOffset);
}

XhciTrb CreateLinkTrb(uint64_t physical_address) {
  return {
      .parameter = physical_address,
      .status = 0,
      .type_and_cycle = (uint16_t)(TypeToInt(TrbType::Link) | kTrb_ToggleCycle),
      .control = 0,
  };
}

XhciTrb CreateEnableSlotTrb() {
  return {
      .parameter = 0,
      .status = 0,
      // FIXME: Accept Cycle Bit as a parameter.
      .type_and_cycle = (uint16_t)(TypeToInt(TrbType::EnableSlot) | kTrb_Cycle),
      // FIXME: Specify slot type if necessary. (XHCI Table 7-9)?
      .control = 0,
  };
}

XhciTrb CreateAddressDeviceCommand(uint64_t input_context, uint8_t slot_id) {
  return {
      .parameter = input_context,
      .status = 0,
      // Always cycle the device straight to addressed.
      .type_and_cycle =
          (uint16_t)(TypeToInt(TrbType::AddressDevice) | kTrb_Cycle),
      .control = (uint16_t)(slot_id << 8),
  };
}

XhciTrb CreateConfigureEndpointCommand(uint64_t input_context,
                                       uint8_t slot_id) {
  return {
      .parameter = input_context,
      .status = 0,
      .type_and_cycle =
          (uint16_t)(TypeToInt(TrbType::ConfigureEndpoint) | kTrb_Cycle),
      .control = (uint16_t)(slot_id << 8),
  };
}

XhciTrb CreateNoOpCommandTrb() {
  return {
      .parameter = 0,
      .status = 0,
      // FIXME: Accept Cycle Bit as a parameter.
      .type_and_cycle =
          (uint16_t)(TypeToInt(TrbType::NoOpCommand) | kTrb_Cycle),
      .control = 0,
  };
}
