#include "input/keyboard.h"

#include <voyageurs/voyageurs.yunq.client.h>
#include <yellowstone/yellowstone.yunq.client.h>
#include <zglobal.h>

#include "util/debug.h"

namespace mmth {
namespace {

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
  YellowstoneClient client(gInitEndpointCap);

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
  None n;
  check(vclient.RegisterKeyboardListener(listn, n));
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
    Keycode k = ScancodeToKeycode(scancode);
    Action a = ScancodeToAction(scancode);
    HandleKeycode(k, a);
  }
}

void KeyboardListenerBase::HandleKeycode(Keycode code, Action action) {
  char c = '\0';

  if (action == kPressed) {
    if (code >= kA && code <= kZ) {
      const char* alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
      c = alpha[code - kA];
    } else if (code >= k1 && code <= k0) {
      const char* num = "1234567890";
      c = num[code - k1];
    } else if (code == kEnter) {
      c = '\n';
    } else if (code == kSpace) {
      c = ' ';
    }
  }

  if (c != '\0') {
    HandleCharacter(c);
  }
}

Keycode KeyboardListenerBase::ScancodeToKeycode(uint8_t scancode) {
  // Cancel out the released bit.
  scancode &= 0x7F;
  switch (scancode) {
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
  }

  dbgln("Unknown scancode {x}", scancode);

  return kUnknownKeycode;
}

Action KeyboardListenerBase::ScancodeToAction(uint8_t scancode) {
  return (scancode & 0x80) ? kReleased : kPressed;
}

}  // namespace mmth
