#include "scheduler/thread.h"

#include "common/gdt.h"
#include "debug/debug.h"
#include "loader/elf_loader.h"
#include "memory/paging_util.h"
#include "scheduler/process.h"
#include "scheduler/scheduler.h"

namespace {

extern "C" void jump_user_space(uint64_t rip, uint64_t rsp);

extern "C" void thread_init() {
  asm("sti");
  gScheduler->CurrentThread().Init();
  panic("Reached end of thread.");
}

}  // namespace

SharedPtr<Thread> Thread::RootThread(Process& root_proc) {
  return new Thread(root_proc);
}

Thread::Thread(Process& proc, uint64_t tid, uint64_t entry)
    : process_(proc), id_(tid), rip_(entry) {
  uint64_t* stack_ptr = proc.vmm().AllocateKernelStack();
  // 0: rip
  *(stack_ptr) = reinterpret_cast<uint64_t>(thread_init);
  // 1-4: rax, rcx, rdx, rbx
  // 5: rbp
  *(stack_ptr - 5) = reinterpret_cast<uint64_t>(stack_ptr + 1);
  // 6-15: rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15
  // 16: cr3
  *(stack_ptr - 16) = proc.vmm().cr3();
  rsp0_ = reinterpret_cast<uint64_t>(stack_ptr - 16);
  rsp0_start_ = reinterpret_cast<uint64_t>(stack_ptr);
}

uint64_t Thread::pid() const { return process_.id(); }

void Thread::Init() {
  dbgln("Thread start.", pid(), id_);
  uint64_t rsp = process_.vmm().AllocateUserStack();
  SetRsp0(rsp0_start_);
  jump_user_space(rip_, rsp);
}

void Thread::Exit() {
  dbgln("Exiting", pid(), id_);
  state_ = FINISHED;
  process_.CheckState();
  gScheduler->Yield();
}
