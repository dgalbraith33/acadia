#pragma once

#include <glacier/container/linked_list.h>
#include <stdint.h>

// KernelStackManager doles out kernel stacks.
//
// KernelStacks are in the region:
// 0xFFFFFFFF 90000000 - 0xFFFFFFFF 9FFFFFFF
//
// Each kernel stack is 12 KiB with a 4 Kib page boundary.
//
// It is  global object that is only exposed via internal linkage
// to the VirtualMemory class. All kernel stacks should be created through that
// class.
class KernelStackManager {
 public:
  KernelStackManager();

  void SetupInterruptStack();

  uint64_t AllocateKernelStack();

  void FreeKernelStack(uint64_t stack_base);

 private:
  uint64_t next_stack_addr_;

  glcr::LinkedList<uint64_t> free_stacks_;
};
