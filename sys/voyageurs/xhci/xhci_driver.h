#pragma once

#include <glacier/container/array.h>
#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/proc/thread.h>
#include <mammoth/util/memory_region.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "xhci/device_slot.h"
#include "xhci/trb_ring.h"
#include "xhci/xhci.h"

class XhciDriver {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<XhciDriver>> InitiateDriver(
      yellowstone::YellowstoneClient& yellowstone);

  XhciDriver(const XhciDriver&) = delete;
  XhciDriver(XhciDriver&&) = default;

  void DumpDebugInfo();

  void InterruptLoop();

 private:
  // MMIO Structures.
  mmth::OwnedMemoryRegion pci_region_;
  PciDeviceHeader* pci_device_header_;

  mmth::OwnedMemoryRegion mmio_regions_;
  volatile XhciCapabilities* capabilities_;
  volatile XhciOperational* operational_;
  // TODO: Extended Capabilities.
  volatile XhciRuntime* runtime_;
  volatile XhciDoorbells* doorbells_;

  // Host Memory Regions.
  TrbRingWriter command_ring_;

  mmth::OwnedMemoryRegion device_context_base_array_mem_;
  uint64_t* device_context_base_array_;

  mmth::OwnedMemoryRegion event_ring_segment_table_mem_;
  XhciEventRingSegmentTableEntry* event_ring_segment_table_;

  TrbRingReader event_ring_;
  Thread interrupt_thread_;

  glcr::Array<DeviceSlot> devices_;

  XhciDriver(mmth::OwnedMemoryRegion&& pci_space);

  glcr::ErrorCode ParseMmioStructures();
  glcr::ErrorCode FreeExistingMemoryStructures() { return glcr::OK; }

  glcr::ErrorCode ResetController();
  void StartInterruptThread();

  glcr::ErrorCode InitiateCommandRing();
  glcr::ErrorCode InitiateDeviceContextBaseArray();
  glcr::ErrorCode InitiateEventRingSegmentTable();

  glcr::ErrorCode InitiateDevices();

  glcr::ErrorCode NoOpCommand();

  void HandleCommandCompletion(const XhciTrb& command_completion_trb);
  void HandleTransferCompletion(const XhciTrb& transfer_event_trb);

  void InitializeSlot(uint8_t slot_index);
};
