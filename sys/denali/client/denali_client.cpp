#include "denali/denali_client.h"

#include <mammoth/debug.h>

#include "denali/denali.h"

MappedMemoryRegion DenaliClient::ReadSectors(uint64_t device_id, uint64_t lba,
                                             uint64_t num_sectors) {
  DenaliRead read{
      .device_id = device_id,
      .lba = lba,
      .size = num_sectors,
  };
  check(channel_.WriteStruct(&read));

  DenaliReadResponse resp;
  uint64_t mem_cap;
  check(channel_.ReadStructAndCap(&resp, &mem_cap));

  return MappedMemoryRegion::FromCapability(mem_cap);
}
