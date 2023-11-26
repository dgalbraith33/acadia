#include "keyboard/keyboard_driver.h"

#include <mammoth/util/debug.h>

void InterruptEnter(void* void_keyboard) {
  KeyboardDriver* keyboard = static_cast<KeyboardDriver*>(void_keyboard);

  keyboard->InterruptLoop();
}

KeyboardDriver::KeyboardDriver() { check(ZIrqRegister(kZIrqKbd, &irq_cap_)); }

void KeyboardDriver::RegisterListener(uint64_t port_cap) {
  listeners_.PushFront(mmth::PortClient::AdoptPort(port_cap));
}

Thread KeyboardDriver::StartInterruptLoop() {
  return Thread(InterruptEnter, this);
}

void KeyboardDriver::InterruptLoop() {
  dbgln("Interrupt");
  while (true) {
    uint8_t scancode;
    uint64_t num_bytes = 1;
    uint64_t num_caps = 0;
    check(ZPortRecv(irq_cap_, &num_bytes, &scancode, &num_caps, nullptr));

    for (mmth::PortClient& client : listeners_) {
      client.WriteByte(scancode);
    }
  }
}
