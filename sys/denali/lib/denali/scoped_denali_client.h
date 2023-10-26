#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/memory_region.h>

#include "denali.yunq.client.h"

class ScopedDenaliClient : protected DenaliClient {
 public:
  ScopedDenaliClient(z_cap_t endpoint_cap, uint64_t device_id,
                     uint64_t lba_offset)
      : DenaliClient(endpoint_cap),
        device_id_(device_id),
        lba_offset_(lba_offset) {}

  glcr::ErrorOr<MappedMemoryRegion> ReadSectors(uint64_t lba,
                                                uint64_t num_sectors) {
    ReadRequest req;
    req.set_device_id(device_id_);
    req.set_lba(lba_offset_ + lba);
    req.set_size(num_sectors);

    ReadResponse resp;
    RET_ERR(DenaliClient::Read(req, resp));

    return MappedMemoryRegion::FromCapability(resp.memory());
  }

 private:
  uint64_t device_id_;
  uint64_t lba_offset_;
};
