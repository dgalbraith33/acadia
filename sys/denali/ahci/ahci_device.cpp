#include "ahci/ahci_device.h"

#include <glacier/status/error.h>
#include <mammoth/debug.h>
#include <string.h>
#include <zcall.h>

AhciDevice::AhciDevice(AhciPort* port) : port_struct_(port) {
  if ((port_struct_->sata_status & 0x103) != 0x103) {
    return;
  }
  uint64_t cl_page = port_struct_->command_list_base & (~0xFFF);
  uint64_t fis_page = port_struct_->fis_base & (~0xFFF);

  if (cl_page != fis_page) {
    crash("Non adjacent cl & fis", glcr::UNIMPLEMENTED);
  }

  command_structures_ = MappedMemoryRegion::DirectPhysical(cl_page, 0x1000);

  uint64_t cl_off = port_struct_->command_list_base & 0xFFF;
  command_list_ =
      reinterpret_cast<CommandList*>(command_structures_.vaddr() + cl_off);

  uint64_t fis_off = port_struct_->fis_base & 0xFFF;
  received_fis_ =
      reinterpret_cast<ReceivedFis*>(command_structures_.vaddr() + fis_off);

  // FIXME: Hacky
  uint64_t ct_off =
      command_list_->command_headers[0].command_table_base_addr & 0xFFF;
  command_table_ =
      reinterpret_cast<CommandTable*>(command_structures_.vaddr() + ct_off);

  port_struct_->interrupt_enable = 0xFFFFFFFF;
}

glcr::ErrorCode AhciDevice::IssueCommand(Command* command) {
  command->PopulateFis(command_table_->command_fis);
  command->PopulatePrdt(command_table_->prdt);

  command_list_->command_headers[0].command =
      (sizeof(HostToDeviceRegisterFis) / 2) & 0x1F;
  command_list_->command_headers[0].prd_table_length = 1;
  command_list_->command_headers[0].prd_byte_count = 0;

  commands_[0] = command;

  commands_issued_ |= 1;
  port_struct_->command_issue |= 1;

  return glcr::OK;
}

void AhciDevice::DumpInfo() {
  dbgln("Comlist: %lx", port_struct_->command_list_base);
  dbgln("FIS: %lx", port_struct_->fis_base);
  dbgln("Command: %x", port_struct_->command);
  dbgln("Signature: %x", port_struct_->signature);
  dbgln("SATA status: %x", port_struct_->sata_status);
  dbgln("Int status: %x", port_struct_->interrupt_status);
  dbgln("Int enable: %x", port_struct_->interrupt_enable);

  // Just dump one command info for now.
  for (uint64_t i = 0; i < 1; i++) {
    dbgln("Command Header: %u", i);
    dbgln("Command %x", command_list_->command_headers[i].command);
    dbgln("PRD Len: %x", command_list_->command_headers[i].prd_table_length);
    dbgln("Command Table %lx",
          command_list_->command_headers[i].command_table_base_addr);
  }
}

void AhciDevice::HandleIrq() {
  uint64_t int_status = port_struct_->interrupt_status;
  // FIXME: Probably only clear the interrupts we know how to handle.
  port_struct_->interrupt_status = int_status;

  uint32_t commands_finished = commands_issued_ & ~port_struct_->command_issue;

  // FIXME: Pass error codes to the callback.
  for (uint64_t i = 0; i < 32; i++) {
    if (commands_finished & (1 << i)) {
      commands_issued_ &= ~(1 << i);
      commands_[i]->Callback();
    }
  }

  // TODO: Do something with this information.
  if (int_status & 0x1) {
    // Device to host.
    DeviceToHostRegisterFis& fis = received_fis_->device_to_host_register_fis;
    if (fis.fis_type != FIS_TYPE_REG_D2H) {
      dbgln("BAD FIS TYPE (exp,act): %x, %x", FIS_TYPE_REG_D2H, fis.fis_type);
      return;
    }
    if (fis.error) {
      dbgln("D2H err: %x", fis.error);
      dbgln("status: %x", fis.status);
    }
  }
  if (int_status & 0x2) {
    // PIO.
    PioSetupFis& fis = received_fis_->pio_set_fis;
    if (fis.fis_type != FIS_TYPE_PIO_SETUP) {
      dbgln("BAD FIS TYPE (exp,act): %x, %x", FIS_TYPE_PIO_SETUP, fis.fis_type);
      return;
    }
    if (fis.error) {
      dbgln("PIO err: %x", fis.error);
    }
  }
}
