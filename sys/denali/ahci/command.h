#pragma once

#include <mammoth/sync/semaphore.h>
#include <mammoth/util/memory_region.h>
#include <stdint.h>

#include "ahci/ahci.h"

class AhciPort;

struct CommandInfo {
  uint8_t command;
  uint64_t lba;
  uint16_t sectors;
  uint64_t paddr;
};

class Command {
 public:
  Command() = default;
  virtual ~Command();
  virtual void PopulateFis(uint8_t* command_fis) const = 0;
  virtual void PopulatePrdt(PhysicalRegionDescriptor* prdt) const = 0;
};

class DmaReadCommand : public Command {
 public:
  DmaReadCommand(uint64_t lba, uint64_t sector_cnt, uint64_t dest_paddr);

  virtual ~DmaReadCommand() override;

  void PopulateFis(uint8_t* command_fis) const override;
  void PopulatePrdt(PhysicalRegionDescriptor* prdt) const override;

 private:
  uint64_t lba_;
  uint64_t sector_cnt_;
  uint64_t paddr_;
};
