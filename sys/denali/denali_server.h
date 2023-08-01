#pragma once

#include <glacier/status/error.h>
#include <mammoth/endpoint_server.h>

#include "ahci/ahci_driver.h"
#include "denali/denali.h"

class DenaliServer : public EndpointServer {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<DenaliServer>> Create(
      AhciDriver& driver);

  void HandleResponse(ResponseContext& response, uint64_t lba, uint64_t size,
                      z_cap_t cap);

  virtual glcr::ErrorCode HandleRequest(RequestContext& request,
                                        ResponseContext& response) override;

 private:
  static const uint64_t kBuffSize = 1024;
  uint8_t read_buffer_[kBuffSize];

  AhciDriver& driver_;

  DenaliServer(z_cap_t endpoint_cap, AhciDriver& driver)
      : EndpointServer(endpoint_cap), driver_(driver) {}

  glcr::ErrorCode HandleRead(DenaliReadRequest* request,
                             ResponseContext& context);
};
