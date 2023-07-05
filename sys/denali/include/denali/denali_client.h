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

class ScopedDenaliClient : protected DenaliClient {
 public:
  ScopedDenaliClient(glcr::UniquePtr<EndpointClient> endpoint,
                     uint64_t device_id, uint64_t lba_offset)
      : DenaliClient(glcr::Move(endpoint)),
        device_id_(device_id),
        lba_offset_(lba_offset) {}

  glcr::ErrorOr<MappedMemoryRegion> ReadSectors(uint64_t lba,
                                                uint64_t num_sectors) {
    return DenaliClient::ReadSectors(device_id_, lba_offset_ + lba,
                                     num_sectors);
  }

 private:
  uint64_t device_id_;
  uint64_t lba_offset_;
};
