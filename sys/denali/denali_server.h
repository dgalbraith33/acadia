#pragma once

#include <glacier/status/error.h>
#include <mammoth/endpoint_server.h>

#include "ahci/ahci_driver.h"
#include "denali/denali.h"

class DenaliServer {
 public:
  DenaliServer(glcr::UniquePtr<EndpointServer> server, AhciDriver& driver);

  glcr::ErrorCode RunServer();

  void HandleResponse(z_cap_t reply_port, uint64_t lba, uint64_t size,
                      z_cap_t cap);

 private:
  static const uint64_t kBuffSize = 1024;
  glcr::UniquePtr<EndpointServer> server_;
  uint8_t read_buffer_[kBuffSize];

  AhciDriver& driver_;

  glcr::ErrorCode HandleRead(const DenaliRead& read, z_cap_t reply_port);
};
