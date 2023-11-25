#include "syscall/thread.h"

#include "capability/capability.h"
#include "debug/debug.h"
#include "scheduler/scheduler.h"

namespace {

bool IsKernel(uint64_t addr) { return (addr & 0xFFFF'8000'0000'0000); }

}  // namespace

glcr::ErrorCode ThreadCreate(ZThreadCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->proc_cap);
  RET_ERR(ValidateCapability<Process>(cap, kZionPerm_SpawnThread));

  auto parent_proc = cap->obj<Process>();
  auto thread = parent_proc->CreateThread();
  *req->thread_cap = curr_proc.AddNewCapability(thread);
  return glcr::OK;
}

glcr::ErrorCode ThreadStart(ZThreadStartReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->thread_cap);
  RET_ERR(ValidateCapability<Thread>(cap, kZionPerm_Write));

  auto thread = cap->obj<Thread>();

  if (IsKernel(req->entry) || IsKernel(req->arg1) || IsKernel(req->arg2)) {
    dbgln("Thread start invalid arg: rip {x}, rdi {x}, rsi {x}", req->entry,
          req->arg1, req->arg2);
    return glcr::INVALID_ARGUMENT;
  }

  thread->Start(req->entry, req->arg1, req->arg2);
  return glcr::OK;
}

glcr::ErrorCode ThreadExit(ZThreadExitReq*) {
  auto curr_thread = gScheduler->CurrentThread();
  curr_thread->Exit();
  panic("Returned from thread exit");
  UNREACHABLE
}

glcr::ErrorCode ThreadWait(ZThreadWaitReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->thread_cap);
  RET_ERR(ValidateCapability<Thread>(cap, kZionPerm_Read));
  auto thread = cap->obj<Thread>();
  thread->Wait();
  return glcr::OK;
}
