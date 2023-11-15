#include "ahci/command.h"

#include <string.h>

#include "ahci/ahci.h"

Command::~Command() {}

DmaReadCommand::DmaReadCommand(uint64_t lba, uint64_t sector_cnt,
                               uint64_t paddr, Mutex& callback_mutex)
    : lba_(lba),
      sector_cnt_(sector_cnt),
      paddr_(paddr),
      callback_mutex_(callback_mutex) {}

DmaReadCommand::~DmaReadCommand() {}

void DmaReadCommand::PopulateFis(uint8_t* command_fis) {
  HostToDeviceRegisterFis fis{
      .fis_type = FIS_TYPE_REG_H2D,
      .pmp_and_c = 0x80,
      .command = 0x25,
      .featurel = 0,

      .lba0 = static_cast<uint8_t>(lba_ & 0xFF),
      .lba1 = static_cast<uint8_t>((lba_ >> 8) & 0xFF),
      .lba2 = static_cast<uint8_t>((lba_ >> 16) & 0xFF),
      .device = (1 << 6),  // ATA LBA Mode

      .lba3 = static_cast<uint8_t>((lba_ >> 24) & 0xFF),
      .lba4 = static_cast<uint8_t>((lba_ >> 32) & 0xFF),
      .lba5 = static_cast<uint8_t>((lba_ >> 40) & 0xFF),
      .featureh = 0,

      .count = static_cast<uint16_t>(sector_cnt_),
      .icc = 0,
      .control = 0,

      .reserved = 0,
  };

  uint64_t bytes = sector_cnt_ * 512;

  memcpy(command_fis, &fis, sizeof(fis));
}
void DmaReadCommand::PopulatePrdt(PhysicalRegionDescriptor* prdt) {
  prdt[0].region_address = paddr_;
  prdt[0].byte_count = sector_cnt_ * 512;
}
void DmaReadCommand::Callback() { callback_mutex_.Release(); }
