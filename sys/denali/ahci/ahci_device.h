#pragma once

#include <glacier/status/error.h>
#include <mammoth/util/memory_region.h>
#include <ztypes.h>

#include "ahci/ahci.h"
#include "ahci/command.h"

class AhciDevice {
 public:
  AhciDevice() {}
  // Caller retains ownership of the pointer.
  AhciDevice(AhciPort* port_struct);

  void DumpInfo();

  bool IsInit() { return port_struct_ != nullptr && command_structures_; }

  glcr::ErrorCode IssueCommand(Command* command);

  void HandleIrq();

  AhciDevice(const AhciDevice&) = delete;
  AhciDevice& operator=(const AhciDevice&) = delete;

 private:
  AhciPort* port_struct_ = nullptr;
  OwnedMemoryRegion command_structures_;

  CommandList* command_list_ = nullptr;
  ReceivedFis* received_fis_ = nullptr;
  CommandTable* command_tables_ = nullptr;

  Command* commands_[32];
  volatile uint32_t commands_issued_ = 0;
};
