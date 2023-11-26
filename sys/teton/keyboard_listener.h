#include <mammoth/input/keyboard.h>

#include "framebuffer/console.h"

class KeyboardListener : public mmth::KeyboardListenerBase {
 public:
  KeyboardListener(Console& c) : mmth::KeyboardListenerBase(), console_(c) {}
  virtual void HandleCharacter(char c) override;

 private:
  Console& console_;
};
