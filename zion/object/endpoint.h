#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>
#include <glacier/status/error.h>

#include "lib/message_queue.h"
#include "lib/mutex.h"
#include "object/kernel_object.h"

class Endpoint;
class ReplyPort;

template <>
struct KernelObjectTag<Endpoint> {
  static const uint64_t type = KernelObject::ENDPOINT;
};

class Endpoint : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::ENDPOINT; }
  static glcr::RefPtr<Endpoint> Create();

  glcr::ErrorCode Write(uint64_t num_bytes, const void* data,
                        z_cap_t reply_port_cap);

  glcr::ErrorCode Read(uint64_t* num_bytes, void* data,
                       z_cap_t* reply_port_cap);

 private:
  Mutex mutex_{"endpoint"};
  UnboundedMessageQueue message_queue_;

  glcr::IntrusiveList<Thread> blocked_threads_;

  Endpoint() {}
};
