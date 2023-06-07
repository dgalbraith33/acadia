#pragma once

#include <stdint.h>
#include <zerrors.h>

class Channel {
 public:
  Channel() {}
  void adopt_cap(uint64_t id);
  uint64_t release_cap();

  z_err_t WriteStr(const char* msg);
  z_err_t ReadStr(char* buffer, uint64_t* size);

  // FIXME: Close channel here.
  ~Channel() {}

 private:
  uint64_t chan_cap_ = 0;
};

uint64_t CreateChannels(Channel& c1, Channel& c2);
