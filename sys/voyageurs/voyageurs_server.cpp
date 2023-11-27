#include "voyageurs_server.h"

glcr::ErrorOr<glcr::UniquePtr<VoyageursServer>> VoyageursServer::Create(
    KeyboardDriver& keyboard_driver) {
  z_cap_t cap;
  RET_ERR(ZEndpointCreate(&cap));
  return glcr::UniquePtr<VoyageursServer>(
      new VoyageursServer(cap, keyboard_driver));
}

glcr::ErrorCode VoyageursServer::HandleRegisterKeyboardListener(
    const KeyboardListener& listener) {
  keyboard_driver_.RegisterListener(listener.port_capability());
  return glcr::OK;
}

VoyageursServer::VoyageursServer(z_cap_t voyageurs_cap,
                                 KeyboardDriver& keyboard_driver)
    : VoyageursServerBase(voyageurs_cap), keyboard_driver_(keyboard_driver) {}
