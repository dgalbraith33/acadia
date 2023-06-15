#pragma once

#include "ahci/ahci_driver.h"
#include "denali/denali.h"

class DenaliServer {
 public:
  DenaliServer(uint64_t channel_cap, AhciDriver& driver);

  z_err_t RunServer();

 private:
  static const uint64_t kBuffSize = 1024;
  uint64_t channel_cap_;
  uint8_t read_buffer_[kBuffSize];

  AhciDriver& driver_;

  z_err_t HandleRead(const DenaliRead& read, DenaliReadResponse& resp,
                     uint64_t& mem_cap);
};
