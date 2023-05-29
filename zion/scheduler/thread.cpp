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
  sched::CurrentThread().Init();
  panic("Reached end of thread.");
}

}  // namespace

Thread* Thread::RootThread(Process* root_proc) { return new Thread(root_proc); }

Thread::Thread(Process* proc, uint64_t tid, uint64_t elf_ptr)
    : process_(proc), id_(tid), elf_ptr_(elf_ptr) {
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
  rsp0_start_ = reinterpret_cast<uint64_t>(stack_ptr);
}

uint64_t Thread::pid() { return process_->id(); }

void Thread::Init() {
  dbgln("[%u.%u]", pid(), id_);
  uint64_t rip = LoadElfProgram(elf_ptr_, 0);
  uint64_t rsp = 0x80000000;
  EnsureResident(rsp - 1, 1);
  SetRsp0(rsp0_start_);
  jump_user_space(rip, rsp);
}

void Thread::Exit() {
  dbgln("[%u.%u] Exiting", pid(), id_);
  state_ = FINISHED;
  process_->CheckState();
  sched::Yield();
}
