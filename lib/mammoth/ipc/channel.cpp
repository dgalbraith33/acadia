#include "ipc/channel.h"

#include <zcall.h>

#include "util/debug.h"

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
    crash("Adopting over channel.", glcr::ALREADY_EXISTS);
  }
  chan_cap_ = id;
}
z_cap_t Channel::release_cap() {
  z_cap_t cap = chan_cap_;
  chan_cap_ = 0;
  return cap;
}

z_cap_t Channel::cap() { return chan_cap_; }

z_err_t Channel::WriteStr(const char* msg) {
  if (!chan_cap_) {
    return glcr::NULL_PTR;
  }
  return ZChannelSend(chan_cap_, strlen(msg), msg, 0, nullptr);
}

z_err_t Channel::ReadStr(char* buffer, uint64_t* size) {
  if (!chan_cap_) {
    return glcr::NULL_PTR;
  }
  uint64_t num_caps = 0;
  return ZChannelRecv(chan_cap_, size, reinterpret_cast<uint8_t*>(buffer),
                      &num_caps, nullptr);
}

z_err_t CreateChannels(Channel& c1, Channel& c2) {
  z_cap_t chan1, chan2;
  RET_ERR(ZChannelCreate(&chan1, &chan2));

  c1.adopt_cap(chan1);
  c2.adopt_cap(chan2);
  return glcr::OK;
}
