#include "syscall/thread.h"

#include "scheduler/scheduler.h"
#include "syscall/syscall.h"

z_err_t ThreadCreate(ZThreadCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->proc_cap);
  RET_ERR(ValidateCap(cap, ZC_PROC_SPAWN_THREAD));

  auto parent_proc = cap->obj<Process>();
  RET_IF_NULL(parent_proc);
  auto thread = parent_proc->CreateThread();
  *req->thread_cap = curr_proc.AddNewCapability(thread, ZC_WRITE);
  return Z_OK;
}

z_err_t ThreadStart(ZThreadStartReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->thread_cap);
  RET_ERR(ValidateCap(cap, ZC_WRITE));

  auto thread = cap->obj<Thread>();
  RET_IF_NULL(thread);
  // FIXME: validate entry point is in user space.
  thread->Start(req->entry, req->arg1, req->arg2);
  return Z_OK;
}

z_err_t ThreadExit(ZThreadExitReq*) {
  auto curr_thread = gScheduler->CurrentThread();
  curr_thread->Exit();
  panic("Returned from thread exit");
}
