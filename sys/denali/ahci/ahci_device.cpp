#include "ahci/ahci_device.h"

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
    crash("Non adjacent cl & fis", Z_ERR_UNIMPLEMENTED);
  }

  check(ZMemoryObjectCreatePhysical(cl_page, 0x1000, &vmmo_cap_));

  uint64_t vaddr;
  check(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, vmmo_cap_, &vaddr));

  uint64_t cl_off = port_struct_->command_list_base & 0xFFF;
  command_list_ = reinterpret_cast<CommandList*>(vaddr + cl_off);

  uint64_t fis_off = port_struct_->fis_base & 0xFFF;
  received_fis_ = reinterpret_cast<ReceivedFis*>(vaddr + fis_off);

  // FIXME: Hacky
  uint64_t ct_off =
      command_list_->command_headers[0].command_table_base_addr & 0xFFF;
  command_table_ = reinterpret_cast<CommandTable*>(vaddr + ct_off);

  port_struct_->interrupt_enable = 0xFFFFFFFF;
}

z_err_t AhciDevice::SendIdentify(uint16_t** result) {
  HostToDeviceRegisterFis fis{
      .fis_type = FIS_TYPE_REG_H2D,
      .pmp_and_c = 0x80,
      .command = 0xEC,
      .featurel = 0,

      .lba0 = 0,
      .lba1 = 0,
      .lba2 = 0,
      .device = 0,

      .lba3 = 0,
      .lba4 = 0,
      .lba5 = 0,
      .featureh = 0,

      .count = 0,
      .icc = 0,
      .control = 0,

      .reserved = 0,
  };

  command_list_->command_headers[0].command = (sizeof(fis) / 2) & 0x1F;
  command_list_->command_headers[0].prd_table_length = 1;

  memcpy(command_table_->command_fis, &fis, sizeof(fis));

  port_struct_->command_issue |= 1;

  uint64_t vmmo_cap, paddr;
  RET_ERR(ZMemoryObjectCreateContiguous(512, &vmmo_cap, &paddr));

  command_table_->prds[0].region_address = paddr;
  command_table_->prds[0].byte_count = 512;

  uint64_t vaddr;
  RET_ERR(ZAddressSpaceMap(Z_INIT_VMAS_SELF, 0, vmmo_cap, &vaddr));

  *result = reinterpret_cast<uint16_t*>(vaddr);

  return Z_OK;
}

void AhciDevice::DumpInfo() {
  dbgln("Comlist: %lx", port_struct_->command_list_base);
  dbgln("FIS: %lx", port_struct_->fis_base);
  dbgln("Command: %x", port_struct_->command);
  dbgln("Signature: %x", port_struct_->signature);
  dbgln("SATA status: %x", port_struct_->sata_status);
  dbgln("Int status: %x", port_struct_->interrupt_status);
  dbgln("Int enable: %x", port_struct_->interrupt_enable);
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

  dbgln("int receieved: %x", int_status);
}
