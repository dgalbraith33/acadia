#include "denali/denali_client.h"

#include <mammoth/debug.h>

#include "denali/denali.h"

glcr::ErrorOr<MappedMemoryRegion> DenaliClient::ReadSectors(
    uint64_t device_id, uint64_t lba, uint64_t num_sectors) {
  DenaliRead read{
      .device_id = device_id,
      .lba = lba,
      .size = num_sectors,
  };
  auto pair_or = endpoint_->CallEndpoint<DenaliRead, DenaliReadResponse>(read);
  if (!pair_or) {
    return pair_or.error();
  }
  auto pair = pair_or.value();

  DenaliReadResponse& resp = pair.first();
  z_cap_t& mem_cap = pair.second();

  return MappedMemoryRegion::FromCapability(mem_cap);
}
