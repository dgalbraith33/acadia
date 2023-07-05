#pragma once

#include <mammoth/endpoint_client.h>
#include <mammoth/memory_region.h>
#include <ztypes.h>

class YellowstoneStub {
 public:
  explicit YellowstoneStub(z_cap_t yellowstone_cap);

  glcr::ErrorOr<MappedMemoryRegion> GetAhciConfig();

 private:
  glcr::UniquePtr<EndpointClient> yellowstone_stub_;
};
