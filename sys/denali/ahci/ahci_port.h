#pragma once

#include <glacier/container/array.h>
#include <glacier/container/array_view.h>
#include <glacier/status/error.h>
#include <glacier/status/error_or.h>
#include <mammoth/sync/semaphore.h>
#include <mammoth/util/memory_region.h>
#include <ztypes.h>

#include "ahci/ahci.h"
#include "ahci/command.h"

class AhciPort {
 public:
  AhciPort() {}
  // Caller retains ownership of the pointer.
  AhciPort(AhciPortHba* port_struct);

  void DumpInfo();

  bool IsSata() { return port_struct_->signature == 0x101; }
  bool IsInit() { return is_init_; }

  glcr::ErrorCode Identify();

  glcr::ErrorOr<mmth::Semaphore*> IssueRead(uint64_t lba, uint16_t sector_cnt,
                                            uint64_t paddr);

  void HandleIrq();

  AhciPort(const AhciPort&) = delete;
  AhciPort& operator=(const AhciPort&) = delete;

 private:
  volatile AhciPortHba* port_struct_ = nullptr;
  mmth::OwnedMemoryRegion command_structures_;

  volatile CommandList* command_list_ = nullptr;
  volatile ReceivedFis* received_fis_ = nullptr;
  glcr::ArrayView<CommandTable> command_tables_;

  glcr::Array<mmth::Semaphore> command_signals_;
  uint32_t commands_issued_ = 0;

  bool is_init_ = false;
  uint64_t lba_count_ = 0;
  uint32_t sector_size_ = 0;

  glcr::ErrorOr<mmth::Semaphore*> IssueCommand(const CommandInfo& command);
};
