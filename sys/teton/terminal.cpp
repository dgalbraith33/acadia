#include "terminal.h"

#include <glacier/string/str_format.h>
#include <glacier/string/str_split.h>
#include <mammoth/file/file.h>

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
  auto tokens = glcr::StrSplit(command, ' ');
  if (tokens.size() == 0) {
    console_.WriteChar('>');
    return;
  }
  glcr::StringView cmd = tokens[0];
  if (cmd == "help") {
    console_.WriteString("Available Commands: cwd\n");
  } else if (cmd == "cwd") {
    console_.WriteString(cwd_);
    console_.WriteChar('\n');
  } else if (cmd == "ls") {
    glcr::StringView directory;
    if (tokens.size() > 1) {
      directory = tokens[1];
    } else {
      directory = cwd_;
    }
    auto files_or = mmth::ListDirectory(directory);
    if (!files_or.ok()) {
      console_.WriteString(glcr::StrFormat("Error: {}\n", files_or.error()));
    } else {
      auto& files = files_or.value();
      for (uint64_t i = 0; i < files.size(); i++) {
        console_.WriteString(files[i]);
        console_.WriteChar('\n');
      }
    }
  } else {
    console_.WriteString("Unknown command: ");
    console_.WriteString(command);
    console_.WriteChar('\n');
  }
  console_.WriteChar('>');
}
