#include "memory/user_stack_manager.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

uint64_t UserStackManager::NewUserStack() {
  if (!freed_stacks_.empty()) {
    return freed_stacks_.PopFront();
  }

  next_stack_ -= kUserStackSize;
  uint64_t stack = next_stack_;
  if (stack <= kUserStackMin) {
    panic("Out of user stacks!");
  }
  return stack;
}

void UserStackManager::FreeUserStack(uint64_t stack_base) {
  if (stack_base & (kUserStackSize - 1)) {
    dbgln("WARN freeing unaligned user stack {x}", stack_base);
  }
  freed_stacks_.PushBack(stack_base);
}
