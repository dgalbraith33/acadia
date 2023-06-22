#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/endpoint_client.h>
#include <mammoth/memory_region.h>

class DenaliClient {
 public:
  DenaliClient(const EndpointClient& endpoint) : endpoint_(endpoint) {}

  glcr::ErrorOr<MappedMemoryRegion> ReadSectors(uint64_t device_id,
                                                uint64_t lba,
                                                uint64_t num_sectors);

 private:
  EndpointClient endpoint_;
};
