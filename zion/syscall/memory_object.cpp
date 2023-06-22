#include "syscall/memory_object.h"

#include "boot/acpi.h"
#include "memory/physical_memory.h"
#include "scheduler/scheduler.h"

z_err_t MemoryObjectCreate(ZMemoryObjectCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  *req->vmmo_cap = curr_proc.AddNewCapability(
      glcr::MakeRefCounted<MemoryObject>(req->size), ZC_WRITE);
  return glcr::OK;
}

z_err_t MemoryObjectCreatePhysical(ZMemoryObjectCreatePhysicalReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t paddr = req->paddr;
  auto vmmo_ref = glcr::MakeRefCounted<FixedMemoryObject>(paddr, req->size);
  *req->vmmo_cap = curr_proc.AddNewCapability(
      StaticCastRefPtr<MemoryObject>(vmmo_ref), ZC_WRITE);
  return glcr::OK;
}

z_err_t MemoryObjectCreateContiguous(ZMemoryObjectCreateContiguousReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  uint64_t paddr = phys_mem::AllocateContinuous(((req->size - 1) / 0x1000) + 1);
  auto vmmo_ref = glcr::MakeRefCounted<FixedMemoryObject>(paddr, req->size);
  *req->vmmo_cap = curr_proc.AddNewCapability(
      StaticCastRefPtr<MemoryObject>(vmmo_ref), ZC_WRITE);
  *req->paddr = paddr;
  return glcr::OK;
}

z_err_t TempPcieConfigObjectCreate(ZTempPcieConfigObjectCreateReq* req) {
  auto& curr_proc = gScheduler->CurrentProcess();
  ASSIGN_OR_RETURN(PcieConfiguration config, GetPciExtendedConfiguration());
  auto vmmo_ref =
      glcr::MakeRefCounted<FixedMemoryObject>(config.base, config.offset);
  *req->vmmo_cap = curr_proc.AddNewCapability(
      StaticCastRefPtr<MemoryObject>(vmmo_ref), ZC_WRITE);
  *req->vmmo_size = config.offset;
  return glcr::OK;
}
