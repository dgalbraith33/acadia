#pragma once

#include <glacier/memory/ref_ptr.h>

#include "lib/message_queue.h"
#include "lib/mutex.h"
#include "object/kernel_object.h"

class ReplyPort;

template <>
struct KernelObjectTag<ReplyPort> {
  static const uint64_t type = KernelObject::REPLY_PORT;
};

class ReplyPort : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::REPLY_PORT; }
  static glcr::RefPtr<ReplyPort> Create();

  uint64_t Write(uint64_t num_bytes, const void* data, uint64_t num_caps,
                 uint64_t* caps);
  uint64_t Read(uint64_t* num_bytes, void* data, uint64_t* num_caps,
                uint64_t* caps);

 private:
  Mutex mutex_{"reply_port"};
  SingleMessageQueue message_holder_;

  glcr::RefPtr<Thread> blocked_thread_;

  ReplyPort() {}
};
