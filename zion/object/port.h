#pragma once

#include "lib/linked_list.h"
#include "object/kernel_object.h"
#include "usr/zcall_internal.h"

class Port : public KernelObject {
 public:
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
};
