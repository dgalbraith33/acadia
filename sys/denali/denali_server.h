#pragma once

#include <glacier/status/error.h>
#include <mammoth/endpoint_server.h>

#include "ahci/ahci_driver.h"
#include "lib/denali/denali.yunq.server.h"

class DenaliServer : public DenaliServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<DenaliServer>> Create(
      AhciDriver& driver);

  glcr::ErrorCode HandleRead(const ReadRequest& req,
                             ReadResponse& resp) override;
  glcr::ErrorCode HandleReadMany(const ReadManyRequest& req,
                                 ReadResponse& resp) override;

 private:
  static const uint64_t kBuffSize = 1024;
  uint8_t read_buffer_[kBuffSize];

  AhciDriver& driver_;

  DenaliServer(z_cap_t endpoint_cap, AhciDriver& driver)
      : DenaliServerBase(endpoint_cap), driver_(driver) {}
};
