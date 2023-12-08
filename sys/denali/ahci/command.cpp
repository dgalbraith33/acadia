#include "ahci/command.h"

#include <mammoth/util/debug.h>

#include "ahci/ahci.h"

namespace {

void* memcpy(void* dest, const void* src, uint64_t count) {
  uint8_t* d = (uint8_t*)dest;
  const uint8_t* s = (uint8_t*)src;
  for (uint64_t i = 0; i < count; i++) {
    d[i] = s[i];
  }
  return dest;
}

}  // namespace

Command::~Command() {}

void IdentifyDeviceCommand::PopulateFis(uint8_t* command_fis) const {
  HostToDeviceRegisterFis fis __attribute__((aligned(16))){
      .fis_type = FIS_TYPE_REG_H2D,
      .pmp_and_c = 0x80,
      .command = kIdentifyDevice,
      .device = 0,
  };

  memcpy(command_fis, &fis, sizeof(fis));
}

void IdentifyDeviceCommand::PopulatePrdt(PhysicalRegionDescriptor* prdt) const {
  prdt[0].region_address = paddr_;
  prdt[0].byte_count = 0x200 - 1;
  dbgln("paddr: {x}", paddr_);
  uint16_t* ident = reinterpret_cast<uint16_t*>(identify_.vaddr());
  dbgln("vaddr: {x}", identify_.vaddr());
  for (uint32_t i = 0; i < 256; i++) {
    ident[i] = 0;
  }
}

void IdentifyDeviceCommand::OnComplete() {
  uint16_t* ident = reinterpret_cast<uint16_t*>(identify_.vaddr());
  uint32_t* sector_size = reinterpret_cast<uint32_t*>(ident + 117);
  dbgln("Sector size: {}", *sector_size);
  uint64_t* lbas = reinterpret_cast<uint64_t*>(ident + 100);
  dbgln("LBA Count: {}", *lbas);
  // TODO tell the port its sector size.
}

DmaReadCommand::DmaReadCommand(uint64_t lba, uint64_t sector_cnt,
                               uint64_t paddr)
    : lba_(lba), sector_cnt_(sector_cnt), paddr_(paddr) {}

DmaReadCommand::~DmaReadCommand() {}

void DmaReadCommand::PopulateFis(uint8_t* command_fis) const {
  HostToDeviceRegisterFis fis{
      .fis_type = FIS_TYPE_REG_H2D,
      .pmp_and_c = 0x80,
      .command = kDmaReadExt,
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

  memcpy(command_fis, &fis, sizeof(fis));
}

void DmaReadCommand::PopulatePrdt(PhysicalRegionDescriptor* prdt) const {
  prdt[0].region_address = paddr_;
  prdt[0].byte_count = sector_cnt_ * 512;
}
