#pragma once

#include <glacier/container/linked_list.h>
#include <mammoth/ipc/port_client.h>
#include <mammoth/ipc/port_server.h>
#include <mammoth/proc/thread.h>

class KeyboardDriver {
 public:
  KeyboardDriver();
  KeyboardDriver(const KeyboardDriver&) = delete;
  KeyboardDriver(KeyboardDriver&&) = delete;

  void RegisterListener(uint64_t port_cap);

  Thread StartInterruptLoop();
  void InterruptLoop();

 private:
  z_cap_t irq_cap_;
  glcr::LinkedList<mmth::PortClient> listeners_;
};
