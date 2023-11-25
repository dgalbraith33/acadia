#include "memory/user_stack_manager.h"

#include "debug/debug.h"
#include "memory/paging_util.h"

uint64_t UserStackManager::NewUserStack() {
  if (!freed_stacks_.empty()) {
    return freed_stacks_.PopFront();
  }

  uint64_t stack = next_stack_;
  next_stack_ -= kUserStackSize;
  if (stack <= kUserStackMin) {
    panic("Out of user stacks!");
  }
  if (stack == kUserStackMax) {
    // Add a additional page boudary between kernel and user space.
    stack -= 0x1000;
  }
  EnsureResident(stack - 1, 1);
  return stack;
}

void UserStackManager::FreeUserStack(uint64_t stack_ptr) {
  freed_stacks_.PushBack(stack_ptr);
}

bool UserStackManager::IsValidStack(uint64_t vaddr) {
  if (vaddr < next_stack_ || vaddr > (kUserStackMax - 0x1000)) {
    return false;
  }
  // Checks if the address is in the first page of the stack.
  if (vaddr & 0xFF000) {
    return true;
  }
  return false;
}
