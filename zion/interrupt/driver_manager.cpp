#include "interrupt/driver_manager.h"

#include "debug/debug.h"

DriverManager* gDriverManager = nullptr;

DriverManager& DriverManager::Get() { return *gDriverManager; }

DriverManager::DriverManager() { gDriverManager = this; }

void DriverManager::WriteMessage(uint64_t irq_num, IpcMessage&& message) {
  if (!driver_map_.Contains(irq_num)) {
    dbgln("WARN IRQ for {x} with no registered driver", irq_num);
    return;
  }

  driver_map_.at(irq_num)->Send(glcr::Move(message));
}

glcr::ErrorCode DriverManager::RegisterListener(uint64_t irq_num,
                                                glcr::RefPtr<Port> port) {
  if (driver_map_.Contains(irq_num)) {
    return glcr::ALREADY_EXISTS;
  }
  return driver_map_.Insert(irq_num, port);
}
