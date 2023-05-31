#include "memory/user_stack_manager.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

uint64_t UserStackManager::NewUserStack() {
  uint64_t stack = next_stack_;
  next_stack_ -= kStackSize;
  if (stack <= kStackMin) {
    panic("Out of user stacks!");
  }
  if (stack == kStackMax) {
    // Add a additional page boudary between kernel and user space.
    stack -= 0x1000;
  }
  EnsureResident(stack - 1, 1);
  return stack;
}

void UserStackManager::FreeUserStack(uint64_t stack_ptr) {
  freed_stacks_++;
  dbgln("%u freed user stacks", freed_stacks_);
}
