#include "mammoth/port.h"

#include <zcall.h>

#include "mammoth/debug.h"

Port::Port(uint64_t port_cap) : port_cap_(port_cap) {}

z_err_t Port::PollForIntCap(uint64_t *msg, uint64_t *cap) {
  uint64_t bytes, caps;
  RET_ERR(ZPortPoll(port_cap_, sizeof(uint64_t),
                    reinterpret_cast<uint8_t *>(msg), /* num_caps= */ 1, cap,
                    &bytes, &caps));

  if (bytes != sizeof(uint64_t)) {
    return Z_ERR_INVALID;
  }
  if (caps != 1) {
    return Z_ERR_INVALID;
  }
  return Z_OK;
}
