#include "mammoth/port.h"

#include <zcall.h>

#include "mammoth/debug.h"

Port::Port(uint64_t port_cap) : port_cap_(port_cap) {}

z_err_t Port::PollForIntCap(uint64_t *msg, uint64_t *cap) {
  uint64_t bytes = sizeof(uint64_t);
  uint64_t caps = 1;
  RET_ERR(ZPortPoll(port_cap_, &bytes, reinterpret_cast<uint8_t *>(msg), &caps,
                    cap));

  if (bytes != sizeof(uint64_t)) {
    return Z_ERR_INVALID;
  }
  if (caps != 1) {
    return Z_ERR_INVALID;
  }
  return Z_OK;
}
