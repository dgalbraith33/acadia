#include "keyboard/keyboard_driver.h"

#include <mammoth/util/debug.h>

namespace {

char ScancodeToChar(uint8_t code) {
  switch (code) {
    case 0x02:
      return '1';
    case 0x03:
      return '2';
    case 0x04:
      return '3';
    case 0x05:
      return '4';
    case 0x06:
      return '5';
    case 0x07:
      return '6';
    case 0x08:
      return '7';
    case 0x09:
      return '8';
    case 0x0A:
      return '9';
    case 0x0B:
      return '0';
    case 0x10:
      return 'Q';
    case 0x11:
      return 'W';
    case 0x12:
      return 'E';
    case 0x13:
      return 'R';
    case 0x14:
      return 'T';
    case 0x15:
      return 'Y';
    case 0x16:
      return 'U';
    case 0x17:
      return 'I';
    case 0x18:
      return 'O';
    case 0x19:
      return 'P';
    case 0x1E:
      return 'A';
    case 0x1F:
      return 'S';
    case 0x20:
      return 'D';
    case 0x21:
      return 'F';
    case 0x22:
      return 'G';
    case 0x23:
      return 'H';
    case 0x24:
      return 'J';
    case 0x25:
      return 'K';
    case 0x26:
      return 'L';
    case 0x2C:
      return 'Z';
    case 0x2D:
      return 'X';
    case 0x2E:
      return 'Y';
    case 0x2F:
      return 'C';
    case 0x30:
      return 'V';
    case 0x31:
      return 'B';
    case 0x32:
      return 'N';
    case 0x33:
      return 'M';
  }

  dbgln("Unhandled scancode {x}", code);
  return '\0';
}

}  // namespace

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
    dbgln("Scan {x}", scancode);

    for (mmth::PortClient& client : listeners_) {
      client.WriteByte(ScancodeToChar(scancode));
    }
  }
}
