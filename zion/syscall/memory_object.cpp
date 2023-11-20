#include "syscall/memory_object.h"

#include "boot/acpi.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"

z_err_t MemoryObjectCreate(ZMemoryObjectCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  *req->vmmo_cap =
      curr_proc.AddNewCapability(glcr::MakeRefCounted<MemoryObject>(req->size));
  return glcr::OK;
}

z_err_t MemoryObjectCreatePhysical(ZMemoryObjectCreatePhysicalReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t paddr = req->paddr;
  auto vmmo_ref =
      glcr::MakeRefCounted<FixedMemoryObject>(paddr, req->size, false);
  *req->vmmo_cap =
      curr_proc.AddNewCapability(StaticCastRefPtr<MemoryObject>(vmmo_ref));
  return glcr::OK;
}

z_err_t MemoryObjectCreateContiguous(ZMemoryObjectCreateContiguousReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t num_pages = ((req->size - 1) / 0x1000) + 1;
  uint64_t paddr = phys_mem::AllocateContinuous(num_pages);
  auto vmmo_ref =
      glcr::MakeRefCounted<FixedMemoryObject>(paddr, req->size, true);
  *req->vmmo_cap =
      curr_proc.AddNewCapability(StaticCastRefPtr<MemoryObject>(vmmo_ref));
  *req->paddr = paddr;
  return glcr::OK;
}

z_err_t MemoryObjectDuplicate(ZMemoryObjectDuplicateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto vmmo_cap = curr_proc.GetCapability(req->vmmo_cap);
  RET_ERR(ValidateCapability<MemoryObject>(vmmo_cap, kZionPerm_Duplicate));

  ASSIGN_OR_RETURN(
      glcr::RefPtr<MemoryObject> new_vmmo,
      vmmo_cap->obj<MemoryObject>()->Duplicate(req->base_offset, req->length));
  *req->new_vmmo_cap =
      curr_proc.AddNewCapability(new_vmmo, vmmo_cap->permissions());
  return glcr::OK;
}

z_err_t MemoryObjectInspect(ZMemoryObjectInspectReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto vmmo_cap = curr_proc.GetCapability(req->vmmo_cap);
  RET_ERR(ValidateCapability<MemoryObject>(vmmo_cap, kZionPerm_Read));

  auto vmmo = vmmo_cap->obj<MemoryObject>();
  *req->size = vmmo->size();

  return glcr::OK;
}
