#pragma once

#include "xhci/xhci.h"

enum class TrbType : uint8_t {
  Reserved = 0,

  // Transfers
  Normal = 1,
  SetupStage = 2,
  DataStage = 3,
  StatusStage = 4,
  Isoch = 5,
  Link = 6,
  EventData = 7,
  NoOp = 8,

  // Commands
  EnableSlot = 9,
  AddressDevice = 11,
  ConfigureEndpoint = 12,
  NoOpCommand = 23,

  // Events
  Transfer = 32,
  CommandCompletion = 33,
  PortStatusChange = 34,
};

TrbType GetType(const XhciTrb& trb);

XhciTrb CreateNormalTrb(uint64_t physical_address, uint8_t size);
XhciTrb CreateLinkTrb(uint64_t physical_address);

XhciTrb CreateEnableSlotTrb();
XhciTrb CreateAddressDeviceCommand(uint64_t input_context, uint8_t slot_id);
XhciTrb CreateConfigureEndpointCommand(uint64_t input_context, uint8_t slot_id);
XhciTrb CreateNoOpCommandTrb();
