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
  if (command == "help") {
    console_.WriteString("Available Commands: cwd\n");
  } else if (command == "cwd") {
    console_.WriteString(cwd_);
    console_.WriteChar('\n');
  } else {
    console_.WriteString("Unknown command: ");
    console_.WriteString(command);
    console_.WriteChar('\n');
  }
  console_.WriteChar('>');
}
