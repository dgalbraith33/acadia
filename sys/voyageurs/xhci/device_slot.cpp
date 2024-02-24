#include "xhci/device_slot.h"

#include <glacier/status/error.h>
#include <mammoth/util/debug.h>

#include "xhci/trb.h"
#include "xhci/xhci_driver.h"

void DeviceSlot::EnableAndInitializeDataStructures(
    XhciDriver* driver, uint8_t slot_index, uint64_t* output_context,
    volatile uint32_t* doorbell) {
  enabled_ = true;
  xhci_driver_ = driver;
  slot_index_ = slot_index;
  doorbell_ = doorbell;

  context_memory_ =
      mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &context_phys_);

  device_context_ =
      reinterpret_cast<XhciDeviceContext*>(context_memory_.vaddr());
  *output_context = context_phys_;
  input_context_ = reinterpret_cast<XhciInputContext*>(context_memory_.vaddr() +
                                                       kInputSlotContextOffset);

  control_endpoint_transfer_trb_ = glcr::MakeUnique<TrbRingWriter>();
}

XhciTrb DeviceSlot::CreateAddressDeviceCommand(uint8_t root_port,
                                               uint32_t route_string,
                                               uint16_t max_packet_size) {
  // Initialize Slot Context and Endpoint 0 Context.
  input_context_->input.add_contexts = 0x3;
  // Set context_entries to 1. XHCI 4.3.3
  input_context_->slot_context.route_speed_entries = (0x1 << 27) | route_string;
  input_context_->slot_context.latency_port_number = root_port << 16;

  // Initialize Control Endpoint.
  input_context_->endpoint_contexts[0].state = 0;
  constexpr uint16_t kCerr = 0x3 << 1;
  constexpr uint16_t kTypeControl = 0x4 << 3;
  input_context_->endpoint_contexts[0].error_and_type =
      kCerr | kTypeControl | (max_packet_size << 16);

  input_context_->endpoint_contexts[0].tr_dequeue_ptr =
      control_endpoint_transfer_trb_->PhysicalAddress() | 0x1;

  endpoints_ = glcr::Array<Endpoint>(32);

  return ::CreateAddressDeviceCommand(context_phys_ + kInputSlotContextOffset,
                                      slot_index_);
}

uint8_t DeviceSlot::State() {
  return device_context_->slot_context.address_and_state >> 27;
}

void DeviceSlot::TransferComplete(uint8_t endpoint_index, uint64_t trb_phys) {
  if (endpoint_index >= 32) {
    dbgln("ERROR: Received transfer for invalid endpoint {x}", endpoint_index);
    return;
  }

  if (endpoint_index != 1) {
    if (!endpoints_[endpoint_index].Enabled()) {
      dbgln("ERROR: XHCI received transfer for disabled endpoint {x}",
            endpoint_index);
      return;
    }
    endpoints_[endpoint_index].TransferComplete(trb_phys);
    return;
  }

  if (!control_completion_sempahores_.Contains(trb_phys)) {
    // Skip this if we don't have a semaphore for it (Setup and Transfer Trbs).
    return;
  }

  control_completion_sempahores_.at(trb_phys)->Signal();
  check(control_completion_sempahores_.Delete(trb_phys));
}

mmth::Semaphore DeviceSlot::IssueConfigureDeviceCommand(
    uint8_t config_value, glcr::UniquePtr<mmth::PortClient> client) {
  input_context_->input.add_contexts = (0x1 << 3) | 0x1;
  input_context_->input.configuration_value = config_value;
  // TODO: Maybe don't hardcode this.
  input_context_->input.interface_number = 0;

  input_context_->slot_context.route_speed_entries &= 0xFFFFFF;

  // TODO: Don't hardcode this.
  uint8_t max_endpoint = 0x3;
  input_context_->slot_context.route_speed_entries |= (max_endpoint << 27);

  // TODO: Dont' hardcode this.
  endpoints_[3].Initialize(input_context_->endpoint_contexts + 2,
                           glcr::Move(client));

  xhci_driver_->IssueCommand(CreateConfigureEndpointCommand(
      context_phys_ + kInputSlotContextOffset, slot_index_));
  return configure_device_semaphore_;
}

void DeviceSlot::SignalConfigureDeviceCompleted() {
  configure_device_semaphore_.Signal();
}
