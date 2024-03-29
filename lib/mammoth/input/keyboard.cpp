#include "input/keyboard.h"

#include <mammoth/util/init.h>
#include <voyageurs/voyageurs.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>

#include "util/debug.h"

namespace mmth {
namespace {

using yellowstone::Endpoint;
using yellowstone::GetEndpointRequest;
using yellowstone::YellowstoneClient;

void KeyboardListenerEntry(void* keyboard_base) {
  reinterpret_cast<KeyboardListenerBase*>(keyboard_base)->ListenLoop();
}

}  // namespace

KeyboardListenerBase::KeyboardListenerBase() {
  auto server_or = PortServer::Create();
  if (!server_or) {
    crash("Failed to create server", server_or.error());
  }
  server_ = server_or.value();
}

void KeyboardListenerBase::Register() {
  uint64_t dup_cap;
  check(ZCapDuplicate(gInitEndpointCap, kZionPerm_All, &dup_cap));
  YellowstoneClient client(dup_cap);

  GetEndpointRequest req;
  req.set_endpoint_name("voyageurs");
  Endpoint endpt;
  check(client.GetEndpoint(req, endpt));

  VoyageursClient vclient(endpt.endpoint());
  KeyboardListener listn;

  // TODO: Create a "ASSIGN_OR_CRASH" macro to simplify this.
  auto client_or = server_.CreateClient();
  if (!client_or.ok()) {
    crash("Failed to create client", client_or.error());
  }
  listn.set_port_capability(client_or.value().cap());
  check(vclient.RegisterKeyboardListener(listn));
}

Thread KeyboardListenerBase::Listen() {
  return Thread(KeyboardListenerEntry, this);
}

void KeyboardListenerBase::ListenLoop() {
  while (true) {
    auto scancode_or = server_.RecvUint16();
    if (!scancode_or.ok()) {
      check(scancode_or.error());
    }
    uint16_t scancode = scancode_or.value();

    Keycode k = ScancodeToKeycode(scancode & 0xFF);
    uint8_t modifiers = (scancode >> 8) & 0xFF;
    HandleKeycode(k, modifiers);
  }
}

void KeyboardListenerBase::HandleKeycode(Keycode code, uint8_t modifiers) {
  char c = '\0';

  if (code >= kA && code <= kZ) {
    if (IsShift(modifiers)) {
      const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      c = alpha[code - kA];

    } else {
      const char* alpha = "abcdefghijklmnopqrstuvwxyz";
      c = alpha[code - kA];
    }
  } else if (code >= k1 && code <= k0) {
    if (IsShift(modifiers)) {
      const char* num = "!@#$%^&*()";
      c = num[code - k1];
    } else {
      const char* num = "1234567890";
      c = num[code - k1];
    }
  } else if (code >= kMinus && code <= kBacktick) {
    if (IsShift(modifiers)) {
      const char* sym = "_+{}|?:\"<>~";
      c = sym[code - kMinus];
    } else {
      const char* sym = "-=[]\\/;',.`";
      c = sym[code - kMinus];
    }
  } else if (code == kEnter) {
    c = '\n';
  } else if (code == kSpace) {
    c = ' ';
  } else if (code == kTab) {
    c = '\t';
  } else if (code == kBackspace) {
    c = '\b';
  }

  if (c != '\0') {
    HandleCharacter(c);
  }
}

Keycode KeyboardListenerBase::ScancodeToKeycode(uint16_t scancode) {
  switch (scancode) {
    case 0x04:
      return kA;
    case 0x05:
      return kB;
    case 0x06:
      return kC;
    case 0x07:
      return kD;
    case 0x08:
      return kE;
    case 0x09:
      return kF;
    case 0x0A:
      return kG;
    case 0x0B:
      return kH;
    case 0x0C:
      return kI;
    case 0x0D:
      return kJ;
    case 0x0E:
      return kK;
    case 0x0F:
      return kL;
    case 0x10:
      return kM;
    case 0x11:
      return kN;
    case 0x12:
      return kO;
    case 0x13:
      return kP;
    case 0x14:
      return kQ;
    case 0x15:
      return kR;
    case 0x16:
      return kS;
    case 0x17:
      return kT;
    case 0x18:
      return kU;
    case 0x19:
      return kV;
    case 0x1A:
      return kW;
    case 0x1B:
      return kX;
    case 0x1C:
      return kY;
    case 0x1D:
      return kZ;
    case 0x1E:
      return k1;
    case 0x1F:
      return k2;
    case 0x20:
      return k3;
    case 0x21:
      return k4;
    case 0x22:
      return k5;
    case 0x23:
      return k6;
    case 0x24:
      return k7;
    case 0x25:
      return k8;
    case 0x26:
      return k9;
    case 0x27:
      return k0;
    case 0x28:
      return kEnter;
    case 0x29:
      return kEsc;
    case 0x2A:
      return kBackspace;
    case 0x2B:
      return kTab;
    case 0x2C:
      return kSpace;
    case 0x2D:
      return kMinus;
    case 0x2E:
      return kEquals;
    case 0x2F:
      return kLBrace;
    case 0x30:
      return kRBrace;
    case 0x31:
      return kBSlash;
    case 0x33:
      return kSemicolon;
    case 0x34:
      return kQuote;
    case 0x35:
      return kBacktick;
    case 0x36:
      return kComma;
    case 0x37:
      return kPeriod;
    case 0x38:
      return kFSlash;
    case 0x39:
      return kEsc;  // Capslock
  }

  dbgln("Unknown scancode {x}", scancode);

  return kUnknownKeycode;
}

}  // namespace mmth
