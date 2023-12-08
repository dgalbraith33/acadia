#pragma once

#include <glacier/container/array_view.h>
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
  volatile AhciPort* port_struct_ = nullptr;
  mmth::OwnedMemoryRegion command_structures_;

  volatile CommandList* command_list_ = nullptr;
  volatile ReceivedFis* received_fis_ = nullptr;
  glcr::ArrayView<CommandTable> command_tables_;

  Command* commands_[32];
  uint32_t commands_issued_ = 0;
};
