#pragma once

#include <glacier/status/error.h>

#include "ahci/ahci_controller.h"
#include "lib/denali/denali.yunq.server.h"

class DenaliServer : public DenaliServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<DenaliServer>> Create(
      AhciController& driver);

  glcr::Status HandleRead(const ReadRequest& req, ReadResponse& resp) override;
  glcr::Status HandleReadMany(const ReadManyRequest& req,
                              ReadResponse& resp) override;

 private:
  static const uint64_t kBuffSize = 1024;
  uint8_t read_buffer_[kBuffSize];

  AhciController& driver_;

  DenaliServer(z_cap_t endpoint_cap, AhciController& driver)
      : DenaliServerBase(endpoint_cap), driver_(driver) {}
};
