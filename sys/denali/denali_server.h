#pragma once

#include <glacier/status/error.h>

#include "ahci/ahci_driver.h"
#include "denali/denali.h"

class DenaliServer {
 public:
  DenaliServer(uint64_t channel_cap, AhciDriver& driver);

  glcr::ErrorCode RunServer();

  void HandleResponse(uint64_t lba, uint64_t size, uint64_t cap);

 private:
  static const uint64_t kBuffSize = 1024;
  uint64_t channel_cap_;
  uint8_t read_buffer_[kBuffSize];

  AhciDriver& driver_;

  glcr::ErrorCode HandleRead(const DenaliRead& read);
};
