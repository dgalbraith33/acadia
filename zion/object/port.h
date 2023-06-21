#pragma once

#include <glacier/container/intrusive_list.h>
#include <glacier/memory/ref_ptr.h>

#include "capability/capability.h"
#include "lib/message_queue.h"
#include "lib/mutex.h"
#include "object/kernel_object.h"
#include "object/thread.h"
#include "usr/zcall_internal.h"

class Port;

template <>
struct KernelObjectTag<Port> {
  static const uint64_t type = KernelObject::PORT;
};

class Port : public KernelObject {
 public:
  uint64_t TypeTag() override { return KernelObject::PORT; }

  Port();

  z_err_t Write(uint64_t num_bytes, const void* bytes, uint64_t num_caps,
                const z_cap_t* caps);
  z_err_t Read(uint64_t* num_bytes, void* bytes, uint64_t* num_caps,
               z_cap_t* caps);

  void WriteKernel(uint64_t init, glcr::RefPtr<Capability> cap);

  bool HasMessages();

 private:
  UnboundedMessageQueue message_queue_;
  glcr::IntrusiveList<Thread> blocked_threads_;

  Mutex mutex_{"Port"};
};
