#pragma once

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
  static void Init();

  uint64_t* AllocateKernelStack();

  void FreeKernelStack(uint64_t stack_base);

 private:
  KernelStackManager();
  uint64_t next_stack_addr_;
  uint64_t freed_stack_cnt_ = 0;
};
