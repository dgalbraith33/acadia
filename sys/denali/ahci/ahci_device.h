#pragma once

#include <zerrors.h>

#include "ahci/ahci.h"

class AhciDevice {
 public:
  AhciDevice() {}
  // Caller retains ownership of the pointer.
  AhciDevice(AhciPort* port_struct);

  void DumpInfo();

  bool IsInit() { return port_struct_ != nullptr && vmmo_cap_ != 0; }

  // Result will point to a 512 byte (256 word array).
  z_err_t SendIdentify(uint16_t** result);

  void HandleIrq();

 private:
  AhciPort* port_struct_ = nullptr;
  uint64_t vmmo_cap_ = 0;

  CommandList* command_list_ = nullptr;
  ReceivedFis* received_fis_ = nullptr;
  CommandTable* command_table_ = nullptr;
};
