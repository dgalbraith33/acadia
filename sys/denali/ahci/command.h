#pragma once

#include <mammoth/memory_region.h>
#include <mammoth/mutex.h>
#include <mammoth/response_context.h>
#include <stdint.h>

#include "ahci/ahci.h"

class Command {
 public:
  virtual ~Command();
  virtual void PopulateFis(uint8_t* command_fis) = 0;
  virtual void PopulatePrdt(PhysicalRegionDescriptor* prdt) = 0;
  virtual void Callback() = 0;
};

class DmaReadCommand : public Command {
 public:
  DmaReadCommand(uint64_t lba, uint64_t sector_cnt, uint64_t dest_paddr,
                 Mutex& callback_mutex);

  virtual ~DmaReadCommand() override;

  void PopulateFis(uint8_t* command_fis) override;
  void PopulatePrdt(PhysicalRegionDescriptor* prdt) override;

  void Callback() override;

 private:
  uint64_t lba_;
  uint64_t sector_cnt_;
  uint64_t paddr_;
  Mutex& callback_mutex_;
};
