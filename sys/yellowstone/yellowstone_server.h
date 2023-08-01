#pragma once

#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>
#include <mammoth/endpoint_server.h>
#include <mammoth/port_server.h>
#include <mammoth/thread.h>

#include "hw/pcie.h"

class YellowstoneServer : public EndpointServer {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<YellowstoneServer>> Create();

  Thread RunRegistration();

  void RegistrationThread();

  virtual glcr::ErrorCode HandleRequest(RequestContext& request,
                                        ResponseContext& response) override;

 private:
  // FIXME: Separate this to its own service.
  PortServer register_port_;

  static const uint64_t kBufferSize = 128;
  uint8_t server_buffer_[kBufferSize];
  char registration_buffer_[kBufferSize];

  // TODO: Store these in a data structure.
  z_cap_t denali_cap_ = 0;
  uint64_t device_id_ = 0;
  uint64_t lba_offset_ = 0;
  z_cap_t victoria_falls_cap_ = 0;

  PciReader pci_reader_;

  YellowstoneServer(z_cap_t endpoint_cap, PortServer port);
};
