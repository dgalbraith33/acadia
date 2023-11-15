#include "object/thread.h"

#include "common/gdt.h"
#include "debug/debug.h"
#include "memory/paging_util.h"
#include "object/process.h"
#include "scheduler/scheduler.h"

#define K_THREAD_DEBUG 0

namespace {

extern "C" void jump_user_space(uint64_t rip, uint64_t rsp, uint64_t arg1,
                                uint64_t arg2);

extern "C" void thread_init() {
  asm("sti");
  gScheduler->CurrentThread()->Init();
  panic("Reached end of thread.");
}

}  // namespace

glcr::RefPtr<Thread> Thread::RootThread(Process& root_proc) {
  return glcr::MakeRefCounted<Thread>(root_proc);
}

glcr::RefPtr<Thread> Thread::Create(Process& proc, uint64_t tid) {
  return glcr::MakeRefCounted<Thread>(proc, tid);
}

Thread::Thread(Process& proc, uint64_t tid) : process_(proc), id_(tid) {
  uint64_t* stack_ptr =
      reinterpret_cast<uint64_t*>(proc.vmas()->AllocateKernelStack());
  // 0: rip
  *(stack_ptr) = reinterpret_cast<uint64_t>(thread_init);
  // 1-4: rax, rcx, rdx, rbx
  // 5: rbp
  *(stack_ptr - 5) = reinterpret_cast<uint64_t>(stack_ptr + 1);
  // 6-15: rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15
  // 16: cr3
  *(stack_ptr - 16) = proc.vmas()->cr3();
  rsp0_ = reinterpret_cast<uint64_t>(stack_ptr - 16);
  rsp0_start_ = reinterpret_cast<uint64_t>(stack_ptr);
}

uint64_t Thread::pid() const { return process_.id(); }

void Thread::Start(uint64_t entry, uint64_t arg1, uint64_t arg2) {
  rip_ = entry;
  arg1_ = arg1;
  arg2_ = arg2;
  state_ = RUNNABLE;
  // Get from parent to avoid creating a new shared ptr.
  gScheduler->Enqueue(process_.GetThread(id_));
}

void Thread::Init() {
#if K_THREAD_DEBUG
  dbgln("Thread start.", pid(), id_);
#endif
  uint64_t rsp = process_.vmas()->AllocateUserStack();
  SetRsp0(rsp0_start_);
  jump_user_space(rip_, rsp, arg1_, arg2_);
}

void Thread::Exit() {
#if K_THREAD_DEBUG
  dbgln("Exiting");
#endif
  state_ = FINISHED;
  process_.CheckState();
  while (blocked_threads_.size() != 0) {
    auto thread = blocked_threads_.PopFront();
    thread->SetState(Thread::RUNNABLE);
    gScheduler->Enqueue(thread);
  }
  gScheduler->Yield();
}

void Thread::Wait() {
  // TODO: We need synchronization code here.
  // Race condition is for A waiting on B.
  // 1. A checks if B is finished.
  // 2. Context Switch A -> B
  // 3. B finishes.
  // 4. Context Switch B -> A
  // 5. A forever blocks on B.
  if (state_ == Thread::FINISHED) {
    return;
  }
  auto thread = gScheduler->CurrentThread();
  thread->SetState(Thread::BLOCKED);
  blocked_threads_.PushBack(thread);
  gScheduler->Yield();
}
