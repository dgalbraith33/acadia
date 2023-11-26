#include <glacier/string/string_builder.h>
#include <mammoth/input/keyboard.h>

#include "framebuffer/console.h"

class Terminal : public mmth::KeyboardListenerBase {
 public:
  Terminal(Console& c) : mmth::KeyboardListenerBase(), console_(c) {}

  virtual void HandleCharacter(char c) override;

  void ExecuteCommand(const glcr::String& command);

 private:
  Console& console_;
  glcr::VariableStringBuilder current_command_;
};
