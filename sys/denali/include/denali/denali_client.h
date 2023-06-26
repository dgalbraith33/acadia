#pragma once

#include <glacier/status/error_or.h>
#include <mammoth/endpoint_client.h>
#include <mammoth/memory_region.h>

class DenaliClient {
 public:
  DenaliClient(glcr::UniquePtr<EndpointClient> endpoint)
      : endpoint_(glcr::Move(endpoint)) {}

  glcr::ErrorOr<MappedMemoryRegion> ReadSectors(uint64_t device_id,
                                                uint64_t lba,
                                                uint64_t num_sectors);

 private:
  glcr::UniquePtr<EndpointClient> endpoint_;
};
