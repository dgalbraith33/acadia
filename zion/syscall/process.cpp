#include "syscall/process.h"

#include "capability/capability.h"
#include "debug/debug.h"
#include "scheduler/process_manager.h"
#include "scheduler/scheduler.h"

z_err_t ProcessExit(ZProcessExitReq* req) {
  auto curr_thread = gScheduler->CurrentThread();
  dbgln("Exit code: {x}", req->code);
  // FIXME: kill process here.
  curr_thread->Exit();
  panic("Returned from thread exit");
  return glcr::UNIMPLEMENTED;
}

z_err_t ProcessSpawn(ZProcessSpawnReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto cap = curr_proc.GetCapability(req->proc_cap);
  RET_ERR(ValidateCapability<Process>(cap, kZionPerm_SpawnProcess));

  glcr::RefPtr<Process> proc = Process::Create();
  gProcMan->InsertProcess(proc);

  *req->new_proc_cap = curr_proc.AddNewCapability(proc);
  *req->new_vmas_cap = curr_proc.AddNewCapability(proc->vmas());

  if (req->bootstrap_cap != 0) {
    auto cap = curr_proc.GetCapability(req->bootstrap_cap);
    if (!cap) {
      return glcr::CAP_NOT_FOUND;
    }
    if (!(cap->HasPermissions(kZionPerm_Transmit))) {
      return glcr::CAP_PERMISSION_DENIED;
    }
    *req->new_bootstrap_cap = proc->AddExistingCapability(
        curr_proc.ReleaseCapability(req->bootstrap_cap));
  }

  return glcr::OK;
}
