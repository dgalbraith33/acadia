#pragma once

#include <denali/denali_client.h>
#include <mammoth/endpoint_client.h>
#include <mammoth/memory_region.h>
#include <mammoth/port_client.h>
#include <ztypes.h>

class YellowstoneStub {
 public:
  explicit YellowstoneStub(z_cap_t yellowstone_cap);

  glcr::ErrorOr<MappedMemoryRegion> GetAhciConfig();
  glcr::ErrorOr<ScopedDenaliClient> GetDenali();

  [[nodiscard]] glcr::ErrorCode Register(glcr::String name,
                                         const EndpointClient& client);

 private:
  glcr::UniquePtr<EndpointClient> yellowstone_stub_;
  PortClient register_port_;
};
