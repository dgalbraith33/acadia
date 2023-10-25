#pragma once

#include <glacier/memory/ref_counted.h>

class KernelObject : public glcr::RefCounted<KernelObject> {
 public:
  enum ObjectType {
    INVALID = 0x0,
    PROCESS = 0x1,
    THREAD = 0x2,
    ADDRESS_SPACE = 0x3,
    MEMORY_OBJECT = 0x4,
    CHANNEL = 0x5,
    PORT = 0x6,
    ENDPOINT = 0x7,
    REPLY_PORT = 0x8,
    MUTEX = 0x9,
  };

  virtual uint64_t TypeTag() = 0;
};

template <typename T>
struct KernelObjectTag {
  static const int type = KernelObject::INVALID;
};
