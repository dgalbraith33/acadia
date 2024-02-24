#include "keyboard/keyboard_driver.h"

#include <mammoth/util/debug.h>

namespace {

KeyboardDriver* gKeyboardDriver = nullptr;

}

void InterruptEnter(void* void_keyboard) {
  KeyboardDriver* keyboard = static_cast<KeyboardDriver*>(void_keyboard);

  keyboard->InterruptLoop();
}

KeyboardDriver::KeyboardDriver() {
  check(ZPortCreate(&port_cap_));
  gKeyboardDriver = this;
}

z_cap_t KeyboardDriver::GetPortCap() { return gKeyboardDriver->port_cap_; }

void KeyboardDriver::RegisterListener(uint64_t port_cap) {
  listeners_.PushFront(mmth::PortClient::AdoptPort(port_cap));
}

Thread KeyboardDriver::StartInterruptLoop() {
  return Thread(InterruptEnter, this);
}

void KeyboardDriver::InterruptLoop() {
  dbgln("Interrupt");
  while (true) {
    uint64_t scancode;
    uint64_t num_bytes = 8;
    uint64_t num_caps = 0;
    check(ZPortRecv(port_cap_, &num_bytes, &scancode, &num_caps, nullptr));

    ProcessInput(scancode);
  }
}

void KeyboardDriver::ProcessInput(uint64_t input) {
  uint64_t new_bitmap = 0;
  for (uint8_t i = 2; i < 8; i++) {
    uint8_t code = (input >> (8 * i)) & 0xFF;
    if (code == 0) {
      break;
    }
    if (code >= 64) {
      dbgln("Skipping keycode: {x}", code);
    }
    uint64_t bit = 1 << code;
    new_bitmap |= bit;
    if ((bitmap_ & bit) != bit) {
      SendKeypress(code);
    }
  }
  bitmap_ = new_bitmap;
}

void KeyboardDriver::SendKeypress(uint8_t scancode) {
  dbgln("{x}", scancode);
  for (mmth::PortClient& client : listeners_) {
    client.WriteByte(scancode);
  }
}
