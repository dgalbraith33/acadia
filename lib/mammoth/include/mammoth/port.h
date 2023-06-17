#pragma once

#include <stdint.h>
#include <zerrors.h>

class Port {
 public:
  Port(uint64_t port_cap);

  z_err_t PollForIntCap(uint64_t* msg, uint64_t* cap);

 private:
  uint64_t port_cap_;
};
