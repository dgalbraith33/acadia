#pragma once

#include "lib/linked_list.h"
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

  z_err_t Write(const ZMessage& msg);
  z_err_t Read(ZMessage& msg);

 private:
  struct Message {
    uint64_t type;
    uint64_t num_bytes;
    uint8_t* bytes;
  };

  LinkedList<Message> pending_messages_;

  LinkedList<RefPtr<Thread>> blocked_threads_;

  Mutex mutex_{"Port"};
};
