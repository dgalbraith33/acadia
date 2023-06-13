#pragma once

#include <stdint.h>

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
  void FreeUserStack(uint64_t stack_ptr);

  // Used to check if we should page in this address.
  bool IsValidStack(uint64_t vaddr);

 private:
  const uint64_t kStackMax = 0x00008000'00000000;
  const uint64_t kStackMin = 0x00007FF0'00000000;
  const uint64_t kStackSize = 0x100000;

  uint64_t next_stack_ = kStackMax;
  uint64_t freed_stacks_ = 0;
};
