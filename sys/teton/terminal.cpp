#include "terminal.h"

void Terminal::HandleCharacter(char c) {
  console_.WriteChar(c);
  if (c == '\n') {
    glcr::String str = current_command_.ToString();
    ExecuteCommand(str);
    current_command_.Reset();
  } else if (c == '\b') {
    current_command_.DeleteLast();
  } else {
    current_command_.PushBack(c);
  }
}

void Terminal::ExecuteCommand(const glcr::String& command) {
  console_.WriteString("Executing: ");
  console_.WriteString(command);
  console_.WriteString("\n>");
}
