#pragma once

#include <mammoth/sync/semaphore.h>
#include <stdint.h>

#include "ahci/ahci.h"

class Command {
 public:
  virtual ~Command();
  virtual void PopulateFis(uint8_t* command_fis) = 0;
  virtual void PopulatePrdt(PhysicalRegionDescriptor* prdt) = 0;
  virtual void WaitComplete() = 0;
  virtual void SignalComplete() = 0;
};

class DmaReadCommand : public Command {
 public:
  DmaReadCommand(uint64_t lba, uint64_t sector_cnt, uint64_t dest_paddr);

  virtual ~DmaReadCommand() override;

  void PopulateFis(uint8_t* command_fis) override;
  void PopulatePrdt(PhysicalRegionDescriptor* prdt) override;

  void WaitComplete() override;
  void SignalComplete() override;

 private:
  uint64_t lba_;
  uint64_t sector_cnt_;
  uint64_t paddr_;
  // TODO: Make this owned by the device so that we don't have to create a new
  // one with the kernel every time a command is issued.
  Semaphore callback_semaphore_;
};
