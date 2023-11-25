#pragma once

#include <glacier/container/linked_list.h>
#include <stdint.h>

#include "memory/constants.h"

// Per-process class to manage user stacks.
//
// User stacks live at
// 0x00007FF0 00000000 - 0x00007FFF FFFFFFFF
// and grow downwards.
//
// Each user stack gets 1 MiB (0x100000 bytes)
// less a page boundary.
// with 1 page allocated at a time.
// TODO: consider increasing this.
class UserStackManager {
 public:
  UserStackManager() {}
  UserStackManager(const UserStackManager&) = delete;

  uint64_t NewUserStack();
  void FreeUserStack(uint64_t stack_base);

 private:
  uint64_t next_stack_ = kUserStackMax;

  glcr::LinkedList<uint64_t> freed_stacks_;
};
