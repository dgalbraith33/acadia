#include "scheduler/thread.h"

#include "debug/debug.h"
#include "scheduler/process.h"
#include "scheduler/scheduler.h"

namespace {

extern "C" void thread_init() {
  asm("sti");
  dbgln("New Thread!");
  sched::Yield();
  panic("End of thread.");
}

}  // namespace

Thread* Thread::RootThread(Process* root_proc) { return new Thread(root_proc); }

Thread::Thread(Process* proc, uint64_t tid) : process_(proc), id_(tid) {
  uint64_t* stack = new uint64_t[512];
  uint64_t* stack_ptr = stack + 511;
  // 0: rip
  *(stack_ptr) = reinterpret_cast<uint64_t>(thread_init);
  // 1-4: rax, rcx, rdx, rbx
  // 5: rbp
  *(stack_ptr - 5) = reinterpret_cast<uint64_t>(stack_ptr + 1);
  // 6-15: rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15
  // 16: cr3
  *(stack_ptr - 16) = proc->cr3();
  rsp0_ = reinterpret_cast<uint64_t>(stack_ptr - 16);
}

uint64_t Thread::pid() { return process_->id(); }
