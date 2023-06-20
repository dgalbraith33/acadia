#pragma once

#include <stdint.h>
#include <zcall.h>

class Channel {
 public:
  Channel() {}
  void adopt_cap(uint64_t id);
  z_cap_t release_cap();
  z_cap_t cap();

  z_err_t WriteStr(const char* msg);
  z_err_t ReadStr(char* buffer, uint64_t* size);

  template <typename T>
  z_err_t WriteStruct(T*);

  template <typename T>
  z_err_t ReadStructAndCap(T*, uint64_t*);

  // FIXME: Close channel here.
  ~Channel() {}

 private:
  z_cap_t chan_cap_ = 0;
};

uint64_t CreateChannels(Channel& c1, Channel& c2);

template <typename T>
z_err_t Channel::WriteStruct(T* obj) {
  return ZChannelSend(chan_cap_, sizeof(T), obj, 0, nullptr);
}

template <typename T>
z_err_t Channel::ReadStructAndCap(T* obj, uint64_t* cap) {
  uint64_t num_bytes = sizeof(T);
  uint64_t num_caps = 1;
  uint64_t ret = ZChannelRecv(chan_cap_, &num_bytes, obj, &num_caps, cap);

  if (ret != Z_OK) {
    return ret;
  }
  if (num_caps != 1 || num_bytes != sizeof(T)) {
    return Z_ERR_INVALID;
  }
  return Z_OK;
}
