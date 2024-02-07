#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/util/memory_region.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "xhci/xhci.h"

class XhciDriver {
 public:
  static glcr::ErrorOr<XhciDriver> InitiateDriver(
      yellowstone::YellowstoneClient& yellowstone);

  void DumpDebugInfo();

 private:
  // MMIO Structures.
  mmth::OwnedMemoryRegion pci_region_;
  PciDeviceHeader* pci_device_header_;

  mmth::OwnedMemoryRegion mmio_regions_;
  XhciCapabilities* capabilities_;
  XhciOperational* operational_;
  // TODO: Extended Capabilities.
  XhciRuntime* runtime_;
  // TODO: Doorbell Array.

  // Host Memory Regions.
  mmth::OwnedMemoryRegion command_ring_mem_;
  XhciCommandTrb* command_trb_;

  mmth::OwnedMemoryRegion device_context_base_array_mem_;
  uint64_t* device_context_base_array_;

  mmth::OwnedMemoryRegion event_ring_segment_table_mem_;
  XhciEventRingSegmentTableEntry* event_ring_segment_table_;

  mmth::OwnedMemoryRegion event_ring_segment_mem_;
  XhciCommandTrb* event_ring_segment_;

  XhciDriver(mmth::OwnedMemoryRegion&& pci_space);

  glcr::ErrorCode ParseMmioStructures();
  glcr::ErrorCode FreeExistingMemoryStructures() { return glcr::OK; }

  glcr::ErrorCode ResetController();

  glcr::ErrorCode InitiateCommandRing();
  glcr::ErrorCode InitiateDeviceContextBaseArray();
  glcr::ErrorCode InitiateEventRingSegmentTable();
};
