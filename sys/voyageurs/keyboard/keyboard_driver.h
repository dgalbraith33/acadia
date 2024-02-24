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

  static z_cap_t GetPortCap();

  void RegisterListener(uint64_t port_cap);

  Thread StartInterruptLoop();
  void InterruptLoop();

 private:
  z_cap_t port_cap_;
  glcr::LinkedList<mmth::PortClient> listeners_;

  uint64_t bitmap_ = 0;

  void ProcessInput(uint64_t input);
  void SendKeypress(uint16_t scancode);
};
