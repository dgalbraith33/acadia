#include "ahci/ahci_port.h"

#include <glacier/status/error.h>
#include <mammoth/util/debug.h>
#include <zcall.h>

AhciPort::AhciPort(AhciPortHba* port) : port_struct_(port) {
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

  commands_issued_ = 0;
  command_signals_ = glcr::Array<mmth::Semaphore>(32);
  for (uint64_t i = 0; i < 32; i++) {
    // This leaves space for 2 prdt entries.
    command_list_->command_headers[i].command_table_base_addr =
        (paddr + 0x500) + (0x100 * i);
  }
  port_struct_->interrupt_enable = 0xFFFFFFFF;
  // kInterrupt_D2H_FIS | kInterrupt_PIO_FIS | kInterrupt_DMA_FIS |
  // kInterrupt_DeviceBits_FIS | kInterrupt_Unknown_FIS;
  port_struct_->sata_error = -1;
  port_struct_->command |= kCommand_Start;
}

glcr::ErrorCode AhciPort::Identify() {
  if (IsSata()) {
    CommandInfo identify{
        .command = kIdentifyDevice,
        .lba = 0,
        .sectors = 1,
        .paddr = 0,
    };
    auto region =
        mmth::OwnedMemoryRegion::ContiguousPhysical(0x200, &identify.paddr);
    ASSIGN_OR_RETURN(auto* sem, IssueCommand(identify));
    sem->Wait();
    uint16_t* ident = reinterpret_cast<uint16_t*>(region.vaddr());
    uint32_t* sector_size = reinterpret_cast<uint32_t*>(ident + 117);
    dbgln("Sector size: {}", *sector_size);
    uint64_t* lbas = reinterpret_cast<uint64_t*>(ident + 100);
    dbgln("LBA Count: {}", *lbas);
  }
  return glcr::OK;
}

glcr::ErrorOr<mmth::Semaphore*> AhciPort::IssueRead(uint64_t lba,
                                                    uint16_t sector_cnt,
                                                    uint64_t paddr) {
  CommandInfo read{
      .command = kDmaReadExt,
      .lba = lba,
      .sectors = sector_cnt,
      .paddr = paddr,
  };
  return IssueCommand(read);
}

glcr::ErrorOr<mmth::Semaphore*> AhciPort::IssueCommand(
    const CommandInfo& command) {
  uint64_t slot;
  for (slot = 0; slot < 32; slot++) {
    if (!(commands_issued_ & (1 << slot))) {
      break;
    }
  }
  if (slot == 32) {
    dbgln("All slots full");
    return glcr::INTERNAL;
  }

  auto* fis = reinterpret_cast<HostToDeviceRegisterFis*>(
      command_tables_[slot].command_fis);
  *fis = HostToDeviceRegisterFis{
      .fis_type = FIS_TYPE_REG_H2D,
      .pmp_and_c = 0x80,
      .command = command.command,
      .featurel = 0,

      .lba0 = static_cast<uint8_t>(command.lba & 0xFF),
      .lba1 = static_cast<uint8_t>((command.lba >> 8) & 0xFF),
      .lba2 = static_cast<uint8_t>((command.lba >> 16) & 0xFF),
      .device = (1 << 6),  // ATA LBA Mode

      .lba3 = static_cast<uint8_t>((command.lba >> 24) & 0xFF),
      .lba4 = static_cast<uint8_t>((command.lba >> 32) & 0xFF),
      .lba5 = static_cast<uint8_t>((command.lba >> 40) & 0xFF),
      .featureh = 0,

      .count = command.sectors,
      .icc = 0,
      .control = 0,

      .reserved = 0,
  };

  command_tables_[slot].prdt[0].region_address = command.paddr;
  command_tables_[slot].prdt[0].byte_count = 512 * command.sectors;

  command_list_->command_headers[slot].prd_table_length = 1;
  command_list_->command_headers[slot].command =
      (sizeof(HostToDeviceRegisterFis) / 2) & 0x1F;
  // Set prefetch bit.
  command_list_->command_headers[slot].command |= (1 << 7);

  // TODO: Synchronization-wise we need to ensure this is set in the same
  // critical section as where we select a slot.
  commands_issued_ |= (1 << slot);
  port_struct_->command_issue |= (1 << slot);

  return &command_signals_[slot];
}

void AhciPort::DumpInfo() {
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

void AhciPort::HandleIrq() {
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
      dbgln("Error: {x}", port_struct_->sata_error);
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
    dbgln("Count: {x} {x} {x}", fis.counth, fis.countl, fis.e_status);
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
      // TODO: Pass error codes to the callback.
      command_signals_[i].Signal();
      commands_issued_ &= ~(1 << i);
    }
  }
}
