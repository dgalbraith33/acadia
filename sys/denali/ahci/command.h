#pragma once

#include <mammoth/sync/semaphore.h>
#include <mammoth/util/memory_region.h>
#include <stdint.h>

#include "ahci/ahci.h"

class AhciPort;

class Command {
 public:
  Command() = default;
  virtual ~Command();
  virtual void PopulateFis(uint8_t* command_fis) = 0;
  virtual void PopulatePrdt(PhysicalRegionDescriptor* prdt) = 0;
  void WaitComplete();
  void SignalComplete();

  virtual void OnComplete() {}

 private:
  // TODO: Make this owned by the device so that we don't have to create a new
  // one with the kernel every time a command is issued.
  mmth::Semaphore callback_semaphore_;
};

class IdentifyDeviceCommand : public Command {
 public:
  IdentifyDeviceCommand(AhciPort* port) : port_(port) {}
  virtual void PopulateFis(uint8_t* command_fis) override;
  virtual void PopulatePrdt(PhysicalRegionDescriptor* prdt) override;

  virtual void OnComplete() override;

 private:
  AhciPort* port_;
  uint64_t paddr_;
  mmth::OwnedMemoryRegion identify_ =
      mmth::OwnedMemoryRegion::ContiguousPhysical(0x200, &paddr_);
};

class DmaReadCommand : public Command {
 public:
  DmaReadCommand(uint64_t lba, uint64_t sector_cnt, uint64_t dest_paddr);

  virtual ~DmaReadCommand() override;

  void PopulateFis(uint8_t* command_fis) override;
  void PopulatePrdt(PhysicalRegionDescriptor* prdt) override;

 private:
  uint64_t lba_;
  uint64_t sector_cnt_;
  uint64_t paddr_;
};
