#include "syscall/address_space.h"

#include "capability/capability.h"
#include "scheduler/scheduler.h"

z_err_t AddressSpaceMap(ZAddressSpaceMapReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto vmas_cap = curr_proc.GetCapability(req->vmas_cap);
  auto vmmo_cap = curr_proc.GetCapability(req->vmmo_cap);
  RET_ERR(ValidateCapability<AddressSpace>(vmas_cap, ZC_WRITE));
  RET_ERR(ValidateCapability<MemoryObject>(vmmo_cap, ZC_WRITE));

  auto vmas = vmas_cap->obj<AddressSpace>();
  auto vmmo = vmmo_cap->obj<MemoryObject>();

  // FIXME: Validation necessary.
  if (req->vmas_offset != 0) {
    vmas->MapInMemoryObject(req->vmas_offset, vmmo);
    *req->vaddr = req->vmas_offset;
  } else {
    *req->vaddr = vmas->MapInMemoryObject(vmmo);
  }
  return Z_OK;
}
