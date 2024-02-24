#pragma once

#include <glacier/container/hash_map.h>
#include <glacier/memory/shared_ptr.h>
#include <glacier/memory/unique_ptr.h>
#include <mammoth/sync/semaphore.h>
#include <mammoth/util/debug.h>
#include <mammoth/util/memory_region.h>

#include "xhci/control_command.h"
#include "xhci/trb_ring.h"
#include "xhci/xhci.h"

class XhciDriver;

class DeviceSlot {
 public:
  DeviceSlot() = default;
  DeviceSlot(const DeviceSlot&) = delete;
  DeviceSlot(DeviceSlot&&) = delete;

  void EnableAndInitializeDataStructures(XhciDriver* driver,
                                         uint8_t slot_index_,
                                         uint64_t* output_context,
                                         volatile uint32_t* doorbell);

  XhciTrb CreateAddressDeviceCommand(uint8_t root_port, uint32_t route_string,
                                     uint16_t max_packet_size);

  mmth::Semaphore IssueConfigureDeviceCommand(uint8_t config_value);
  void SignalConfigureDeviceCompleted();

  // Caller must keep the command in scope until it completes.
  template <typename T>
  void ExecuteReadControlCommand(ReadControlCommand<T>& command);

  void TransferComplete(uint8_t endpoint_index, uint64_t trb_phys);

  uint8_t State();

 private:
  bool enabled_ = false;

  XhciDriver* xhci_driver_;
  uint8_t slot_index_ = 0;
  volatile uint32_t* doorbell_ = nullptr;

  uint64_t context_phys_ = 0;
  mmth::OwnedMemoryRegion context_memory_;

  static constexpr uint64_t kInputSlotContextOffset = 0x400;

  XhciDeviceContext* device_context_;
  XhciInputContext* input_context_;

  glcr::UniquePtr<TrbRingWriter> control_endpoint_transfer_trb_;
  glcr::HashMap<uint64_t, glcr::SharedPtr<mmth::Semaphore>>
      control_completion_sempahores_;

  mmth::Semaphore configure_device_semaphore_;

  glcr::UniquePtr<TrbRingWriter> other_endpoint_transfer_trb_;
};

template <typename T>
void DeviceSlot::ExecuteReadControlCommand(ReadControlCommand<T>& command) {
  control_endpoint_transfer_trb_->EnqueueTrb(command.SetupTrb());
  control_endpoint_transfer_trb_->EnqueueTrb(command.DataTrb());
  uint64_t last_phys =
      control_endpoint_transfer_trb_->EnqueueTrb(command.StatusTrb());
  // Ring the control endpoint doorbell.
  *doorbell_ = 1;

  check(control_completion_sempahores_.Insert(last_phys,
                                              command.CompletionSemaphore()));
}
