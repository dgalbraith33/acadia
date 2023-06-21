#include "syscall/process.h"

#include "capability/capability.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"

z_err_t ProcessExit(ZProcessExitReq* req) {
  auto curr_thread = gScheduler->CurrentThread();
  dbgln("Exit code: %x", req->code);
  // FIXME: kill process here.
  curr_thread->Exit();
  panic("Returned from thread exit");
  return Z_ERR_UNIMPLEMENTED;
}

z_err_t ProcessSpawn(ZProcessSpawnReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->proc_cap);
  RET_ERR(ValidateCapability<Process>(cap, ZC_PROC_SPAWN_PROC));

  glcr::RefPtr<Process> proc = Process::Create();
  gProcMan->InsertProcess(proc);

  *req->new_proc_cap = curr_proc.AddNewCapability(
      proc, ZC_PROC_SPAWN_PROC | ZC_PROC_SPAWN_THREAD | ZC_WRITE);
  *req->new_vmas_cap = curr_proc.AddNewCapability(proc->vmas(), ZC_WRITE);

  if (req->bootstrap_cap != 0) {
    auto cap = curr_proc.ReleaseCapability(req->bootstrap_cap);
    if (!cap) {
      return Z_ERR_CAP_NOT_FOUND;
    }
    // FIXME: Check permissions.
    *req->new_bootstrap_cap = proc->AddExistingCapability(cap);
  }

  return Z_OK;
}
