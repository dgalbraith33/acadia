#include "ahci/ahci_device.h"

#include <glacier/status/error.h>
#include <mammoth/util/debug.h>
#include <zcall.h>

AhciDevice::AhciDevice(AhciPort* port) : port_struct_(port) {
  if ((port_struct_->sata_status & 0x103) != 0x103) {
    crash("Creating device on port without a device",
          glcr::FAILED_PRECONDITION);
  }

  // 0x0-0x400 -> Command List
  // 0x400-0x500 -> Received FIS
  // 0x500-0x2500 -> Command Tables (0x100 each) (Max PRDT Length is 8 for now)
  uint64_t paddr;
  command_structures_ =
      mmth::OwnedMemoryRegion::ContiguousPhysical(0x2500, &paddr);

  command_list_ = reinterpret_cast<CommandList*>(command_structures_.vaddr());
  port_struct_->command_list_base = paddr;

  received_fis_ =
      reinterpret_cast<ReceivedFis*>(command_structures_.vaddr() + 0x400);
  port_struct_->fis_base = paddr + 0x400;
  port_struct_->command |= kCommand_FIS_Receive_Enable;

  command_tables_ = glcr::ArrayView(
      reinterpret_cast<CommandTable*>(command_structures_.vaddr() + 0x500), 32);

  for (uint64_t i = 0; i < 32; i++) {
    // This leaves space for 2 prdt entries.
    command_list_->command_headers[i].command_table_base_addr =
        (paddr + 0x500) + (0x100 * i);
    commands_[i] = nullptr;
  }
  port_struct_->interrupt_enable =
      kInterrupt_D2H_FIS | kInterrupt_PIO_FIS | kInterrupt_DMA_FIS |
      kInterrupt_DeviceBits_FIS | kInterrupt_Unknown_FIS;
  port_struct_->sata_error = -1;
  port_struct_->command |= kCommand_Start;
}

glcr::ErrorCode AhciDevice::IssueCommand(Command* command) {
  uint64_t slot;
  for (slot = 0; slot < 32; slot++) {
    if (commands_[slot] == nullptr) {
      break;
    }
  }
  if (slot == 32) {
    dbgln("All slots full");
    return glcr::INTERNAL;
  }
  command->PopulateFis(command_tables_[slot].command_fis);
  command->PopulatePrdt(command_tables_[slot].prdt);

  command_list_->command_headers[slot].command =
      (sizeof(HostToDeviceRegisterFis) / 2) & 0x1F;
  command_list_->command_headers[slot].prd_table_length = 1;
  command_list_->command_headers[slot].prd_byte_count = 0;

  commands_[slot] = command;

  commands_issued_ |= (1 << slot);
  port_struct_->command_issue |= (1 << slot);

  return glcr::OK;
}

void AhciDevice::DumpInfo() {
  dbgln("Comlist: {x}", port_struct_->command_list_base);
  dbgln("FIS: {x}", port_struct_->fis_base);
  dbgln("Command: {x}", port_struct_->command);
  dbgln("Signature: {x}", port_struct_->signature);
  dbgln("SATA status: {x}", port_struct_->sata_status);
  dbgln("SATA error: {x}", port_struct_->sata_error);
  dbgln("Int status: {x}", port_struct_->interrupt_status);
  dbgln("Int enable: {x}", port_struct_->interrupt_enable);
}

bool CheckFisType(FIS_TYPE expected, uint8_t actual) {
  if (expected == actual) {
    return true;
  }
  dbgln("BAD FIS TYPE (exp,act): {x}, {x}", static_cast<uint64_t>(expected),
        static_cast<uint64_t>(actual));
  return false;
}

void AhciDevice::HandleIrq() {
  uint32_t int_status = port_struct_->interrupt_status;
  port_struct_->interrupt_status = int_status;

  bool has_error = false;
  if (int_status & kInterrupt_D2H_FIS) {
    dbgln("D2H Received");
    // Device to host.
    volatile DeviceToHostRegisterFis& fis =
        received_fis_->device_to_host_register_fis;
    if (!CheckFisType(FIS_TYPE_REG_D2H, fis.fis_type)) {
      return;
    }
    if (fis.error) {
      dbgln("D2H err: {x}", fis.error);
      dbgln("status: {x}", fis.status);
      has_error = true;
    }
  }
  if (int_status & kInterrupt_PIO_FIS) {
    dbgln("PIO Received");
    // PIO.
    volatile PioSetupFis& fis = received_fis_->pio_set_fis;
    if (!CheckFisType(FIS_TYPE_PIO_SETUP, fis.fis_type)) {
      return;
    }
    if (fis.error) {
      dbgln("PIO err: {x}", fis.error);
      dbgln("status: {x}", fis.status);
      has_error = true;
    }
  }
  if (int_status & kInterrupt_DMA_FIS) {
    dbgln("DMA Received");
    volatile DmaFis& fis = received_fis_->dma_fis;
    if (!CheckFisType(FIS_TYPE_DMA_SETUP, fis.fis_type)) {
      return;
    }
    // TODO: Actually do something with this FIS.
  }
  if (int_status & kInterrupt_DeviceBits_FIS) {
    dbgln("Device Bits Received");
    volatile SetDeviceBitsFis& fis = received_fis_->set_device_bits_fis;
    if (!CheckFisType(FIS_TYPE_DEV_BITS, fis.fis_type)) {
      return;
    }
    if (fis.error) {
      dbgln("SetDeviceBits err: {x}", fis.error);
      dbgln("status: {x}", fis.status);
      has_error = true;
    }
  }
  if (int_status & kInterrupt_Unknown_FIS) {
    dbgln("Unknown FIS recieved, type: {x}", received_fis_->unknown_fis[0]);
  }
  uint32_t commands_finished = commands_issued_ & ~port_struct_->command_issue;

  for (uint64_t i = 0; i < 32; i++) {
    if (commands_finished & (1 << i)) {
      commands_issued_ &= ~(1 << i);
      // FIXME: Pass error codes to the callback.
      commands_[i]->SignalComplete();
      commands_[i] = nullptr;
    }
  }
}
