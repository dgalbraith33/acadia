#include "terminal.h"

#include <glacier/string/str_format.h>
#include <glacier/string/str_split.h>
#include <mammoth/file/file.h>
#include <mammoth/proc/process.h>
#include <mammoth/util/debug.h>
#include <mammoth/util/init.h>

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
    console_.WriteString("Available Commands: pwd cd ls\n");
  } else if (cmd == "pwd") {
    console_.WriteString(cwd_);
    console_.WriteChar('\n');
  } else if (cmd == "cd") {
    if (tokens.size() != 2) {
      console_.WriteString("Provide an absolute path.\n>");
      return;
    }
    auto files_or = mmth::ListDirectory(tokens[1]);
    if (files_or.ok()) {
      cwd_ = tokens[1];
    } else {
      console_.WriteString(glcr::StrFormat("Error: {}\n", files_or.error()));
    }

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
  } else if (cmd == "exec") {
    if (tokens.size() != 2) {
      console_.WriteString("Provide the name of an executable.\n>");
      return;
    }
    auto file = mmth::File::Open(tokens[1]);

    z_cap_t endpoint;
    if (ZCapDuplicate(gInitEndpointCap, kZionPerm_All, &endpoint) != glcr::OK) {
      console_.WriteString("Couldn't duplicate yellowstone cap for spawn");
      return;
    }

    auto error_or_cap =
        mmth::SpawnProcessFromElfRegion((uint64_t)file.raw_ptr(), endpoint);
    if (!error_or_cap.ok()) {
      console_.WriteString(
          glcr::StrFormat("Error: {}\n", error_or_cap.error()));
      return;
    }
    uint64_t err_code;
    check(ZProcessWait(error_or_cap.value(), &err_code));
    if (err_code != 0) {
      console_.WriteString(glcr::StrFormat(
          "Process Error: {}\n", static_cast<glcr::ErrorCode>(err_code)));
    }

  } else {
    console_.WriteString("Unknown command: ");
    console_.WriteString(command);
    console_.WriteChar('\n');
  }
  console_.WriteChar('>');
}
