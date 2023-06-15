#pragma once

#include <mammoth/memory_region.h>
#include <zerrors.h>

#include "ahci/ahci.h"

class AhciDevice {
 public:
  AhciDevice() {}
  // Caller retains ownership of the pointer.
  AhciDevice(AhciPort* port_struct);

  void DumpInfo();

  bool IsInit() { return port_struct_ != nullptr && command_structures_; }

  z_err_t SendIdentify();
  void HandleIdentify();

  void HandleIrq();

 private:
  AhciPort* port_struct_ = nullptr;
  MappedMemoryRegion command_structures_;

  CommandList* command_list_ = nullptr;
  ReceivedFis* received_fis_ = nullptr;
  CommandTable* command_table_ = nullptr;

  struct Command {
    MappedMemoryRegion region;
    // std::function<void(MappedMemoryRegion)> callback;
  };
  Command commands_[32];
  uint32_t commands_issued_ = 0;
};
