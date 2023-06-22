#include "object/port.h"

#include "scheduler/scheduler.h"

void Port::WriteKernel(uint64_t init, glcr::RefPtr<Capability> cap) {
  MutexHolder h(mutex_);
  message_queue_.WriteKernel(init, cap);
}
