#pragma once

#include <mammoth/memory_region.h>
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
  typedef void (*DmaCallback)(ResponseContext&, uint64_t, uint64_t, z_cap_t);
  DmaReadCommand(uint64_t lba, uint64_t sector_cnt, DmaCallback callback,
                 ResponseContext& reply_port);

  virtual ~DmaReadCommand() override;

  void PopulateFis(uint8_t* command_fis) override;
  void PopulatePrdt(PhysicalRegionDescriptor* prdt) override;

  void Callback() override;

 private:
  ResponseContext& response_;
  uint64_t lba_;
  uint64_t sector_cnt_;
  DmaCallback callback_;
  MappedMemoryRegion region_;
};
