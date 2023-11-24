#include "syscall/address_space.h"

#include "capability/capability.h"
#include "scheduler/scheduler.h"

z_err_t AddressSpaceMap(ZAddressSpaceMapReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto vmas_cap = curr_proc.GetCapability(req->vmas_cap);
  auto vmmo_cap = curr_proc.GetCapability(req->vmmo_cap);
  RET_ERR(ValidateCapability<AddressSpace>(vmas_cap, kZionPerm_Write));
  RET_ERR(ValidateCapability<MemoryObject>(vmmo_cap, kZionPerm_Write));

  auto vmas = vmas_cap->obj<AddressSpace>();
  auto vmmo = vmmo_cap->obj<MemoryObject>();

  // FIXME: Validation necessary.
  if (req->vmas_offset != 0) {
    RET_ERR(vmas->MapInMemoryObject(req->vmas_offset, vmmo));
    *req->vaddr = req->vmas_offset;
  } else {
    ASSIGN_OR_RETURN(*req->vaddr, vmas->MapInMemoryObject(vmmo, req->align));
  }
  return glcr::OK;
}

z_err_t AddressSpaceUnmap(ZAddressSpaceUnmapReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  auto vmas_cap = curr_proc.GetCapability(req->vmas_cap);
  RET_ERR(ValidateCapability<AddressSpace>(vmas_cap, kZionPerm_Write));

  auto vmas = vmas_cap->obj<AddressSpace>();
  return vmas->FreeAddressRange(req->lower_addr, req->upper_addr);
}
