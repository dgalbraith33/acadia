#include "input/keyboard.h"

#include <voyageurs/voyageurs.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>
#include <zglobal.h>

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
    auto scancode_or = server_.RecvChar();
    if (!scancode_or.ok()) {
      check(scancode_or.error());
    }
    uint8_t scancode = scancode_or.value();

    if (scancode == 0xE0) {
      extended_on_ = true;
      continue;
    }

    Keycode k = ScancodeToKeycode(scancode);
    Action a = ScancodeToAction(scancode);
    HandleKeycode(k, a);
  }
}

void KeyboardListenerBase::HandleKeycode(Keycode code, Action action) {
  char c = '\0';

  if (action == kPressed) {
    if (code >= kA && code <= kZ) {
      if (IsShift()) {
        const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        c = alpha[code - kA];

      } else {
        const char* alpha = "abcdefghijklmnopqrstuvwxyz";
        c = alpha[code - kA];
      }
    } else if (code >= k1 && code <= k0) {
      if (IsShift()) {
        const char* num = "!@#$%^&*()";
        c = num[code - k1];
      } else {
        const char* num = "1234567890";
        c = num[code - k1];
      }
    } else if (code >= kMinus && code <= kBacktick) {
      if (IsShift()) {
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
    } else if (code == kLShift) {
      lshift_ = true;
    } else if (code == kRShift) {
      rshift_ = true;
    }
  } else if (action == kReleased) {
    if (code == kLShift) {
      lshift_ = false;
    } else if (code == kRShift) {
      rshift_ = false;
    }
  }

  if (c != '\0') {
    HandleCharacter(c);
  }
}

Keycode KeyboardListenerBase::ScancodeToKeycode(uint8_t scancode) {
  // Cancel out the released bit.
  scancode &= 0x7F;
  if (extended_on_) {
    extended_on_ = false;

    switch (scancode) {
      case 0x1D:
        return kRCtrl;
      case 0x38:
        return kRAlt;
      case 0x48:
        return kUp;
      case 0x4B:
        return kLeft;
      case 0x4D:
        return kRight;
      case 0x50:
        return kDown;
      case 0x53:
        return kDelete;
      case 0x5B:
        return kSuper;
    }
    dbgln("Unknown extended scancode {x}", scancode);

    return kUnknownKeycode;
  }

  switch (scancode) {
    case 0x01:
      return kEsc;
    case 0x02:
      return k1;
    case 0x03:
      return k2;
    case 0x04:
      return k3;
    case 0x05:
      return k4;
    case 0x06:
      return k5;
    case 0x07:
      return k6;
    case 0x08:
      return k7;
    case 0x09:
      return k8;
    case 0x0A:
      return k9;
    case 0x0B:
      return k0;
    case 0x0C:
      return kMinus;
    case 0x0D:
      return kEquals;
    case 0x0E:
      return kBackspace;
    case 0x0F:
      return kTab;
    case 0x10:
      return kQ;
    case 0x11:
      return kW;
    case 0x12:
      return kE;
    case 0x13:
      return kR;
    case 0x14:
      return kT;
    case 0x15:
      return kY;
    case 0x16:
      return kU;
    case 0x17:
      return kI;
    case 0x18:
      return kO;
    case 0x19:
      return kP;
    case 0x1A:
      return kLBrace;
    case 0x1B:
      return kRBrace;
    case 0x1C:
      return kEnter;
    case 0x1D:
      return kLCtrl;
    case 0x1E:
      return kA;
    case 0x1F:
      return kS;
    case 0x20:
      return kD;
    case 0x21:
      return kF;
    case 0x22:
      return kG;
    case 0x23:
      return kH;
    case 0x24:
      return kJ;
    case 0x25:
      return kK;
    case 0x26:
      return kL;
    case 0x27:
      return kSemicolon;
    case 0x28:
      return kQuote;
    case 0x29:
      return kBacktick;
    case 0x2A:
      return kLShift;
    case 0x2B:
      return kBSlash;
    case 0x2C:
      return kZ;
    case 0x2D:
      return kX;
    case 0x2E:
      return kC;
    case 0x2F:
      return kV;
    case 0x30:
      return kB;
    case 0x31:
      return kN;
    case 0x32:
      return kM;
    case 0x33:
      return kComma;
    case 0x34:
      return kPeriod;
    case 0x35:
      return kFSlash;
    case 0x36:
      return kRShift;
    case 0x38:
      return kLAlt;
    case 0x39:
      return kSpace;
  }

  dbgln("Unknown scancode {x}", scancode);

  return kUnknownKeycode;
}

Action KeyboardListenerBase::ScancodeToAction(uint8_t scancode) {
  return (scancode & 0x80) ? kReleased : kPressed;
}

}  // namespace mmth
