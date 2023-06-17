#pragma once

#include <stdint.h>
#include <zcall.h>

class Port {
 public:
  Port(uint64_t port_cap);

  z_err_t PollForIntCap(uint64_t* msg, uint64_t* cap);

  template <typename T>
  z_err_t WriteMessage(const T& obj, uint64_t cap);

 private:
  uint64_t port_cap_;
};

template <typename T>
z_err_t Port::WriteMessage(const T& obj, uint64_t cap) {
  return ZPortSend(port_cap_, sizeof(obj),
                   reinterpret_cast<const uint8_t*>(&obj), 1, &cap);
}
