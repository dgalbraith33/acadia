#pragma once

#include <glacier/status/error_or.h>
#include <ztypes.h>

namespace mmth {

class Mutex {
 public:
  Mutex(const Mutex&) = delete;
  Mutex(Mutex&&);
  Mutex& operator=(Mutex&&);

  static glcr::ErrorOr<Mutex> Create();

  glcr::ErrorCode Lock();
  glcr::ErrorCode Release();

 private:
  z_cap_t mutex_cap_;

  Mutex(z_cap_t mutex_cap) : mutex_cap_(mutex_cap) {}
};

}  // namespace mmth
