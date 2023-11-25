#pragma once

#include <glacier/container/hash_map.h>
#include <glacier/memory/ref_ptr.h>

#include "object/port.h"

class DriverManager {
 public:
  static DriverManager& Get();

  DriverManager();
  DriverManager(const DriverManager&) = delete;
  DriverManager(DriverManager&&) = delete;

  void WriteMessage(uint64_t irq_num, IpcMessage&& message);

  glcr::ErrorCode RegisterListener(uint64_t irq_num, glcr::RefPtr<Port> port);

 private:
  glcr::HashMap<uint64_t, glcr::RefPtr<Port>> driver_map_;
};
