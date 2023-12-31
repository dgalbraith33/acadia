#include "sync/mutex.h"

#include <zcall.h>

namespace mmth {

Mutex::Mutex(Mutex&& other) : mutex_cap_(other.mutex_cap_) {
  other.mutex_cap_ = 0;
}

Mutex& Mutex::operator=(Mutex&& other) {
  // TODO: Release existing mutex if it exists.
  mutex_cap_ = other.mutex_cap_;
  other.mutex_cap_ = 0;
  return *this;
}

glcr::ErrorOr<Mutex> Mutex::Create() {
  z_cap_t mutex_cap;
  RET_ERR(ZMutexCreate(&mutex_cap));
  return Mutex(mutex_cap);
}

glcr::ErrorCode Mutex::Lock() {
  return static_cast<glcr::ErrorCode>(ZMutexLock(mutex_cap_));
}
glcr::ErrorCode Mutex::Release() {
  return static_cast<glcr::ErrorCode>(ZMutexRelease(mutex_cap_));
}

}  // namespace mmth
