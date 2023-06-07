#include "mammoth/channel.h"

#include <zcall.h>

#include "mammoth/debug.h"

namespace {

uint64_t strlen(const char* ptr) {
  uint64_t len = 0;
  while (*ptr != '\0') {
    len++;
    ptr++;
  }
  return len;
}

}  // namespace

void Channel::adopt_cap(uint64_t id) {
  if (chan_cap_ != 0) {
    crash("Adopting over channel.", Z_ERR_EXISTS);
  }
  chan_cap_ = id;
}
uint64_t Channel::release_cap() {
  uint64_t cap = chan_cap_;
  chan_cap_ = 0;
  return cap;
}

z_err_t Channel::WriteStr(const char* msg) {
  if (!chan_cap_) {
    return Z_ERR_NULL;
  }
  uint64_t type = 11;
  return ZChannelSend(chan_cap_, type, strlen(msg),
                      reinterpret_cast<const uint8_t*>(msg), 0, 0);
}

z_err_t Channel::ReadStr(char* buffer, uint64_t* size) {
  if (!chan_cap_) {
    return Z_ERR_NULL;
  }
  uint64_t type;
  uint64_t num_caps;
  return ZChannelRecv(chan_cap_, *size, reinterpret_cast<uint8_t*>(buffer), 0,
                      0, &type, size, &num_caps);
}

z_err_t CreateChannels(Channel& c1, Channel& c2) {
  uint64_t chan1, chan2;
  z_err_t err = ZChannelCreate(&chan1, &chan2);
  if (err != Z_OK) {
    return err;
  }

  c1.adopt_cap(chan1);
  c2.adopt_cap(chan2);
  return Z_OK;
}
