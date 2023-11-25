#pragma once

#include <glacier/memory/unique_ptr.h>
#include <glacier/status/error_or.h>

#include "keyboard/keyboard_driver.h"
#include "voyageurs/voyageurs.yunq.server.h"

class VoyageursServer : public VoyageursServerBase {
 public:
  static glcr::ErrorOr<glcr::UniquePtr<VoyageursServer>> Create(
      KeyboardDriver& keyboard_driver);

  virtual glcr::ErrorCode HandleRegisterKeyboardListener(
      const KeyboardListener& listener, None&) override;

 private:
  KeyboardDriver& keyboard_driver_;
  VoyageursServer(z_cap_t voyageurs_cap, KeyboardDriver& keyboard_driver);
};
