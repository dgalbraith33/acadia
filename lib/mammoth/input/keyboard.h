#pragma once

#include "mammoth/ipc/port_server.h"
#include "mammoth/proc/thread.h"

namespace mmth {

enum Keycode {
  kUnknownKeycode = 0x0,

  kA = 0x1,
  kB = 0x2,
  kC = 0x3,
  kD = 0x4,
  kE = 0x5,
  kF = 0x6,
  kG = 0x7,
  kH = 0x8,
  kI = 0x9,
  kJ = 0xA,
  kK = 0xB,
  kL = 0xC,
  kM = 0xD,
  kN = 0xE,
  kO = 0xF,
  kP = 0x10,
  kQ = 0x11,
  kR = 0x12,
  kS = 0x13,
  kT = 0x14,
  kU = 0x15,
  kV = 0x16,
  kW = 0x17,
  kX = 0x18,
  kY = 0x19,
  kZ = 0x1A,

  k1 = 0x20,
  k2 = 0x21,
  k3 = 0x22,
  k4 = 0x23,
  k5 = 0x24,
  k6 = 0x25,
  k7 = 0x26,
  k8 = 0x27,
  k9 = 0x28,
  k0 = 0x29,

  kSpace = 0x30,
  kEnter = 0x31,
  kTab = 0x32,

  kMinus = 0x40,
  kEquals = 0x41,
  kLBrace = 0x42,
  kRBrace = 0x43,
  kBSlash = 0x44,
  kFSlash = 0x45,
  kSemicolon = 0x46,
  kQuote = 0x47,
  kComma = 0x48,
  kPeriod = 0x49,

  kLShift = 0x50,
  kRShift = 0x51,
};

enum Action {
  kUnknownAction,
  kPressed,
  kReleased,
};

class KeyboardListenerBase {
 public:
  KeyboardListenerBase();
  KeyboardListenerBase(const KeyboardListenerBase&) = delete;
  KeyboardListenerBase(KeyboardListenerBase&&) = delete;

  void Register();

  Thread Listen();

  void ListenLoop();

  // Override this to recieve all raw keycodes. By default
  // this function will try to translate each keycode into
  // a printable character and call HandleCharacter.
  virtual void HandleKeycode(Keycode code, Action action);

  // This function is called by the default HandleKeycode
  // implementation if you do not override it. If it recieves
  // input that corresponds to a printable character it will
  virtual void HandleCharacter(char c){};

 private:
  PortServer server_;

  bool lshift_ = false;
  bool rshift_ = false;

  Keycode ScancodeToKeycode(uint8_t scancode);
  Action ScancodeToAction(uint8_t scancode);

  bool IsShift() { return lshift_ || rshift_; }
};

}  // namespace mmth
