#pragma once

#include <glacier/memory/unique_ptr.h>
#include <mammoth/util/memory_region.h>

#include "xhci/trb_ring.h"
#include "xhci/xhci.h"

class DeviceSlot {
 public:
  DeviceSlot() = default;
  DeviceSlot(const DeviceSlot&) = delete;
  DeviceSlot(DeviceSlot&&) = delete;

  void EnableAndInitializeDataStructures(uint8_t slot_index_,
                                         uint64_t* output_context);

  XhciTrb CreateAddressDeviceCommand(uint8_t root_port, uint32_t route_string,
                                     uint16_t max_packet_size);

  uint8_t State();

 private:
  bool enabled_ = false;

  uint8_t slot_index_ = 0;

  uint64_t context_phys_ = 0;
  mmth::OwnedMemoryRegion context_memory_;

  static constexpr uint64_t kInputSlotContextOffset = 0x400;

  XhciDeviceContext* device_context_;
  XhciInputContext* input_context_;

  glcr::UniquePtr<TrbRingWriter> control_endpoint_transfer_trb_;
};
