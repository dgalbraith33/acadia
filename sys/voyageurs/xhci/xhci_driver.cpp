#include "xhci/xhci_driver.h"

#include <mammoth/util/debug.h>
#include <mammoth/util/memory_region.h>
#include <zcall.h>

#include "xhci/trb.h"
#include "xhci/xhci.h"

void interrupt_thread(void* void_driver) {
  XhciDriver* driver = static_cast<XhciDriver*>(void_driver);

  driver->InterruptLoop();

  crash("Driver returned from interrupt loop", glcr::INTERNAL);
}

glcr::ErrorOr<glcr::UniquePtr<XhciDriver>> XhciDriver::InitiateDriver(
    yellowstone::YellowstoneClient& yellowstone) {
  yellowstone::XhciInfo info;
  check(yellowstone.GetXhciInfo(info));

  mmth::OwnedMemoryRegion pci_region =
      mmth::OwnedMemoryRegion::FromCapability(info.mutable_xhci_region());
  // Have to make this a heap object so that the reference passed to the
  // interrupt loop remains valid.
  glcr::UniquePtr<XhciDriver> driver(new XhciDriver(glcr::Move(pci_region)));
  driver->ParseMmioStructures();
  driver->FreeExistingMemoryStructures();
  driver->ResetController();
  driver->StartInterruptThread();
  dbgln("XHCI CONTROLLER RESET");
  driver->NoOpCommand();
  driver->InitiateDevices();
  return glcr::Move(driver);
}

void XhciDriver::InterruptLoop() {
  while (true) {
    while ((runtime_->interrupters[0].management & 0x1) != 0x1) {
      check(ZThreadSleep(50));
    }
    while (event_ring_.HasNext()) {
      XhciTrb trb = event_ring_.Read();
      uint16_t type = trb.type_and_cycle >> 10;
      switch (type) {
        case 33:
          dbgln("Command Completion Event. {x}", trb.parameter);
          break;
        case 34:
          dbgln("Port Status Change Event, enabling slot.");
          command_ring_.EnqueueTrb(CreateEnableSlotTrb());
          doorbells_->doorbell[0] = 0;
          break;
        default:
          dbgln("Unknown TRB Type {x} received.", type);
          break;
      }
    }

    runtime_->interrupters[0].event_ring_dequeue_pointer =
        event_ring_.DequeuePtr() | 0x8;
    runtime_->interrupters[0].management |= 0x1;
  }
}

void XhciDriver::DumpDebugInfo() {
  dbgln("Code: {x} {x} {x}", pci_device_header_->class_code,
        pci_device_header_->subclass, pci_device_header_->prog_interface);

  dbgln("BAR: {x}", pci_device_header_->bars[0] & ~0xFFF);
  dbgln("In P: {x} L: {x}", pci_device_header_->interrupt_pin,
        pci_device_header_->interrupt_line);

  dbgln("Cap length: {x}", capabilities_->length_and_version & 0xFF);
  dbgln("XHCI Version: {x}",
        (capabilities_->length_and_version & 0xFFFF0000) >> 16);
  dbgln("Max Slots: {x}", capabilities_->hcs_params_1 & 0xFF);
  dbgln("Max Interrupters: {x}", (capabilities_->hcs_params_1 & 0x3FF00) >> 8);
  uint64_t max_ports = (capabilities_->hcs_params_1 & 0xFF00'0000) >> 24;
  dbgln("Params 2: {x}", capabilities_->hcs_params_2);
  dbgln("Params 3: {x}", capabilities_->hcs_params_3);
  dbgln("Max Ports: {x}", max_ports);
  dbgln("Capabilities: {x}", capabilities_->capabilites);
  dbgln("Doorbell: {x}", capabilities_->doorbell_offset);
  dbgln("Runtime: {x}", capabilities_->runtime_offset);

  dbgln("Op cmd: {x} sts: {x}", operational_->usb_command,
        operational_->usb_status);
  dbgln("Page size: {x}", operational_->page_size);
  dbgln("Device Context Base Array: {x}", operational_->device_context_base);
  dbgln("Command Ring Control: {x}", operational_->command_ring_control);
  dbgln("Config: {x}", operational_->configure);

  for (uint64_t i = 0; i < max_ports; i++) {
    XhciPort* port = reinterpret_cast<XhciPort*>(
        reinterpret_cast<uint64_t>(operational_) + 0x400 + (0x10 * i));
    port->status_and_control &= ~0x10000;
    dbgln("Port {x}: {x}", i, port->status_and_control);
    if ((port->status_and_control & 0x3) == 0x1) {
      dbgln("Resetting: {x}", i);
      port->status_and_control |= 0x10;
      doorbells_->doorbell[0] = 0;
    }
  }

  dbgln("Int 0 ES: {x}",
        runtime_->interrupters[0].event_ring_segment_table_base_address);
}

XhciDriver::XhciDriver(mmth::OwnedMemoryRegion&& pci_space)
    : pci_region_(glcr::Move(pci_space)) {}

glcr::ErrorCode XhciDriver::ParseMmioStructures() {
  pci_device_header_ = reinterpret_cast<PciDeviceHeader*>(pci_region_.vaddr());

  // TODO: Officially determine the size of this memory.
  mmio_regions_ = mmth::OwnedMemoryRegion::DirectPhysical(
      pci_device_header_->bars[0] & ~0xFFF, 0x3000);

  capabilities_ = reinterpret_cast<XhciCapabilities*>(mmio_regions_.vaddr());

  uint64_t op_base =
      mmio_regions_.vaddr() + (capabilities_->length_and_version & 0xFF);
  operational_ = reinterpret_cast<XhciOperational*>(op_base);

  runtime_ = reinterpret_cast<XhciRuntime*>(mmio_regions_.vaddr() +
                                            capabilities_->runtime_offset);

  doorbells_ = reinterpret_cast<XhciDoorbells*>(mmio_regions_.vaddr() +
                                                capabilities_->doorbell_offset);

  return glcr::OK;
}

glcr::ErrorCode XhciDriver::ResetController() {
  // Stop the Host Controller.
  // FIXME: Do this before freeing existing structures.
  operational_->usb_command &= ~0x1;

  while ((operational_->usb_status & 0x1) != 0x1) {
    dbgln("Waiting XHCI Halt.");
    RET_ERR(ZThreadSleep(50));
  }

  // Host Controller Reset
  operational_->usb_command |= 0x2;

  while (operational_->usb_command & 0x2) {
    dbgln("Waiting Reset");
    RET_ERR(ZThreadSleep(50));
  }

  while (operational_->usb_command & (0x1 << 11)) {
    dbgln("Waiting controller ready");
    RET_ERR(ZThreadSleep(50));
  }

  InitiateCommandRing();
  InitiateDeviceContextBaseArray();
  InitiateEventRingSegmentTable();

  // Run the controller.
  operational_->usb_command |= 0x1;

  return glcr::OK;
}

void XhciDriver::StartInterruptThread() {
  interrupt_thread_ = Thread(interrupt_thread, this);
}

glcr::ErrorCode XhciDriver::InitiateCommandRing() {
  operational_->command_ring_control = command_ring_.PhysicalAddress();
  dbgln("CRC: {x}", operational_->command_ring_control);
  return glcr::OK;
}

glcr::ErrorCode XhciDriver::InitiateDeviceContextBaseArray() {
  uint64_t dcba_phys;
  device_context_base_array_mem_ =
      mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &dcba_phys);
  device_context_base_array_ =
      reinterpret_cast<uint64_t*>(device_context_base_array_mem_.vaddr());

  operational_->device_context_base = dcba_phys;

  uint64_t max_slots = (0x1000 / sizeof(uint64_t)) - 1;

  if (max_slots > (capabilities_->hcs_params_1 & 0xFF)) {
    max_slots = capabilities_->hcs_params_1 & 0xFF;
  }
  operational_->configure =
      (operational_->configure & ~0xFF) | (max_slots & 0xFF);

  // TODO: Initialize scratchpad if that is needed by the controller.

  return glcr::OK;
}

glcr::ErrorCode XhciDriver::InitiateEventRingSegmentTable() {
  uint64_t erst_phys;
  event_ring_segment_table_mem_ =
      mmth::OwnedMemoryRegion::ContiguousPhysical(0x1000, &erst_phys);

  event_ring_segment_table_ = reinterpret_cast<XhciEventRingSegmentTableEntry*>(
      event_ring_segment_table_mem_.vaddr());

  uint64_t ers_size = 0x1000 / sizeof(XhciTrb);
  event_ring_segment_table_[0].ring_segment_base =
      event_ring_.PhysicalAddress();
  event_ring_segment_table_[0].ring_segment_size = ers_size & 0xFFFF;

  runtime_->interrupters[0].event_ring_dequeue_pointer =
      event_ring_.PhysicalAddress() | 0x8;
  runtime_->interrupters[0].event_ring_segment_table_size = 1;
  runtime_->interrupters[0].event_ring_segment_table_base_address = erst_phys;

  // Enable interrupts.
  runtime_->interrupters[0].management |= 0x2;
  runtime_->interrupters[0].moderation = 4000;
  operational_->usb_command |= 0x4;
  return glcr::OK;
}

glcr::ErrorCode XhciDriver::InitiateDevices() {
  uint64_t max_ports = (capabilities_->hcs_params_1 & 0xFF00'0000) >> 24;
  for (uint64_t i = 0; i < max_ports; i++) {
    XhciPort* port = reinterpret_cast<XhciPort*>(
        reinterpret_cast<uint64_t>(operational_) + 0x400 + (0x10 * i));
    port->status_and_control &= ~0x10000;
    dbgln("Port {x}: {x}", i, port->status_and_control);
    if ((port->status_and_control & 0x3) == 0x1) {
      dbgln("Resetting: {x}", i);
      port->status_and_control |= 0x10;
    }
  }
  return glcr::OK;
}

glcr::ErrorCode XhciDriver::NoOpCommand() {
  command_ring_.EnqueueTrb(CreateNoOpCommandTrb());
  doorbells_->doorbell[0] = 0;
  return glcr::OK;
}
